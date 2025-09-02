// Node.cpp - Simplified ESP-NOW communication node
#include "Node.h"
#include "Router.h"
#include "MessageHandler.h"
#include "NodeService.h"
#include "MacAddressStorage.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>

namespace NuggetsInc
{
    Node *Node::activeInstance = nullptr;

    Node::Node() : peerIntialized(false), InitializeMenu(false)
    {
        // Initialize the peer MAC address to all 0xFF
        memset(peerMAC, 0xFF, sizeof(peerMAC));

        // Initialize the outgoing message queue
        outgoingQueue = xQueueCreate(10, sizeof(struct_message));
        if (outgoingQueue == nullptr) {
            Serial.println("Failed to create outgoing message queue");
        }

        // Initialize the mutex for the ackSemaphores map
        mapMutex = xSemaphoreCreateMutex();
        if (mapMutex == nullptr) {
            Serial.println("Failed to create mutex for ackSemaphores");
        }

        // Create delegated components
        router_ = new Router();
        NodeService* nodeService = new NodeService(this);
        messageHandler_ = new MessageHandler(router_, nodeService, this);
        routeMode_ = RouteMode::AUTO;
    }

    Node::~Node()
    {
        if (outgoingQueue != nullptr)
        {
            vQueueDelete(outgoingQueue);
        }
        if (mapMutex != nullptr)
        {
            vSemaphoreDelete(mapMutex);
        }
        delete router_;
        delete messageHandler_;
    }

    void Node::begin()
    {
        Serial.println("Begin Node Initialization");
        Serial.flush();

        // Initialize MAC address storage
        MacAddressStorage &macStorage = MacAddressStorage::getInstance();
        if (!macStorage.init()) {
            Serial.println("Warning: Failed to initialize MAC address storage");
        }

        activeInstance = this;

        // Initialize WiFi for ESP-NOW
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        WiFi.setSleep(false);
        esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

        Serial.print("Device MAC Address: ");
        Serial.println(WiFi.macAddress());

        // Initialize ESP-NOW
        if (esp_now_init() != ESP_OK) {
            Serial.println("ESP-NOW initialization failed.");
            return;
        }

        // Register callbacks
        esp_now_register_send_cb(onDataSentCallback);
        esp_now_register_recv_cb(onDataRecvCallback);

        // Create outgoing message processing task
        xTaskCreate(processOutgoingQueueTask, "OutgoingQueue", 4096, this, 1, NULL);
    }

    bool Node::sendDataBlocking(const struct_message &msg, uint32_t timeout_ms, int maxRetries)
    {
        if (!peerIntialized) {
            Serial.println("Cannot send data: Peer not initialized");
            return false;
        }

        // Prepare message for sending
        struct_message messageToSend = msg;
        prepareMessage(messageToSend, true);

        // Create ACK semaphore
        SemaphoreHandle_t ackSemaphore = xSemaphoreCreateBinary();
        if (ackSemaphore == nullptr) {
            Serial.println("Failed to create semaphore for ack");
            return false;
        }

        // Register semaphore for ACK
        xSemaphoreTake(mapMutex, portMAX_DELAY);
        ackSemaphores[messageToSend.messageID] = ackSemaphore;
        xSemaphoreGive(mapMutex);

        // Send message
        bool success = false;
        if (xQueueSend(outgoingQueue, &messageToSend, pdMS_TO_TICKS(10)) == pdPASS) {
            success = (xSemaphoreTake(ackSemaphore, pdMS_TO_TICKS(timeout_ms)) == pdPASS);
        }

        // Cleanup
        xSemaphoreTake(mapMutex, portMAX_DELAY);
        ackSemaphores.erase(messageToSend.messageID);
        xSemaphoreGive(mapMutex);
        vSemaphoreDelete(ackSemaphore);

        return success;
    }

    bool Node::sendDataNonBlocking(const struct_message &msg)
    {
        if (!peerIntialized) {
            Serial.println("Cannot send data: Peer not initialized");
            return false;
        }

        struct_message messageToSend = msg;
        prepareMessage(messageToSend, false);

        if (xQueueSend(outgoingQueue, &messageToSend, pdMS_TO_TICKS(10)) != pdPASS) {
            Serial.println("Failed to queue outgoing message (non-blocking)");
            return false;
        }

        return true;
    }

    bool Node::isPeerIntialized()
    {
        return peerIntialized;
    }

    Node *Node::getActiveInstance()
    {
        return activeInstance;
    }

    void Node::onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (activeInstance)
        {
            activeInstance->handleOnDataSent(mac_addr, status);
        }
    }

    void Node::onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        if (activeInstance)
        {
            if (len >= sizeof(struct_message)) {
                struct_message receivedMessage;
                memcpy(&receivedMessage, incomingData, sizeof(struct_message));

                // Null-terminate string fields for safety
                receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
                receivedMessage.data[sizeof(receivedMessage.data) - 1] = '\0';
                receivedMessage.path[sizeof(receivedMessage.path) - 1] = '\0';

                // If a peer wasn't previously connected, add it now
                if (!activeInstance->peerIntialized)
                {
                    memcpy(activeInstance->peerMAC, mac_addr, 6);
                    Serial.print("Sender: Stored peer MAC: ");
                    for (int i = 0; i < 6; i++) {
                        Serial.printf("%02X", activeInstance->peerMAC[i]);
                        if (i < 5) Serial.print(":");
                    }
                    Serial.println();
                    activeInstance->addPeer(mac_addr);
                }

                // Now delegate to MessageHandler
                if (activeInstance->messageHandler_) {
                    activeInstance->messageHandler_->processReceivedMessage(mac_addr, receivedMessage);
                }
            }
        }
    }

    void Node::handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (status != ESP_NOW_SEND_SUCCESS)
        {
            Serial.println("Data send failed");
        }
    }

    void Node::notifyAckReceived(uint32_t messageID)
    {
        xSemaphoreTake(mapMutex, portMAX_DELAY);
        auto it = ackSemaphores.find(messageID);
        if (it != ackSemaphores.end())
        {
            xSemaphoreGive(it->second);
        }
        xSemaphoreGive(mapMutex);
    }

    void Node::setSelfMac(uint8_t out[6])
    {
        String macStr = WiFi.macAddress();
        int b[6];
        if (sscanf(macStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) == 6)
        {
            for (int i = 0; i < 6; i++)
                out[i] = (uint8_t)b[i];
        }
    }

    void Node::addPeer(const uint8_t *mac_addr)
    {
        if (!esp_now_is_peer_exist(mac_addr)) {
            esp_now_peer_info_t peerInfo = {};
            memcpy(peerInfo.peer_addr, mac_addr, 6);
            peerInfo.channel = 1;
            peerInfo.encrypt = false;

            if (esp_now_add_peer(&peerInfo) == ESP_OK) {
                Serial.println("Peer added successfully");
                peerIntialized = true;
            } else {
                Serial.println("Failed to add peer");
            }
        } else {
            peerIntialized = true;
        }
    }

    void Node::prepareMessage(struct_message &msg, bool isBlocking)
    {
        static uint32_t blockingCounter = 0;
        static uint32_t nonBlockingCounter = 0x80000000u;

        msg.messageID = isBlocking ? blockingCounter++ : nonBlockingCounter++;
        setSelfMac(msg.SenderMac);
        memset(msg.destinationMac, 0, sizeof(msg.destinationMac));
        msg.path[0] = '\0';
    }

    void Node::processOutgoingQueueTask(void *pvParameters)
    {
        Node *instance = static_cast<Node *>(pvParameters);
        struct_message message;

        for (;;)
        {
            if (xQueueReceive(instance->outgoingQueue, &message, portMAX_DELAY) == pdPASS)
            {
                if (esp_now_is_peer_exist(instance->peerMAC)) {
                    esp_err_t result = esp_now_send(instance->peerMAC, reinterpret_cast<uint8_t *>(&message), sizeof(message));
                    if (result != ESP_OK)
                    {
                        Serial.printf("Failed to send data via ESP-NOW: %s (%d)\n", esp_err_to_name(result), result);
                    }
                }
            }
        }
    }



} // namespace NuggetsInc
