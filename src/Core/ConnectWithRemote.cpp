// ConnectWithRemote.cpp
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>

namespace NuggetsInc
{

    ConnectWithRemote *ConnectWithRemote::activeInstance = nullptr;

    ConnectWithRemote::ConnectWithRemote()
        : peerConnected(false), shouldStartMainMenu(false)
    {
        memset(peerMAC, 0xFF, sizeof(peerMAC));
        outgoingQueue = xQueueCreate(10, sizeof(struct_message));
        if (outgoingQueue == nullptr)
        {
            Serial.println("Failed to create outgoing message queue");
        }

        mapMutex = xSemaphoreCreateMutex();
        if (mapMutex == nullptr)
        {
            Serial.println("Failed to create mutex for ackSemaphores");
        }
    }

    ConnectWithRemote::~ConnectWithRemote()
    {
        if (outgoingQueue != nullptr)
        {
            vQueueDelete(outgoingQueue);
        }
        if (mapMutex != nullptr)
        {
            vSemaphoreDelete(mapMutex);
        }
    }

    void ConnectWithRemote::begin()
    {
        activeInstance = this;

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);

        Serial.print("Sender MAC Address: ");
        Serial.println(WiFi.macAddress());

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("ESP-NOW initialization failed, retrying...");
            int retries = 3;
            while (esp_now_init() != ESP_OK && retries > 0)
            {
                delay(1000);
                retries--;
            }

            if (retries == 0)
            {
                Serial.println("ESP-NOW initialization failed after retries");
                return;
            }
        }

        esp_now_register_send_cb(onDataSentCallback);
        esp_now_register_recv_cb(onDataRecvCallback);
        Serial.println("ESP-NOW initialized successfully");

        xTaskCreate(
            processOutgoingQueueTask,
            "OutgoingQueueTask",
            4096,
            this,
            1,
            NULL);
    }

    bool ConnectWithRemote::sendDataBlocking(const struct_message &msg, uint32_t timeout_ms)
    {
        if (!peerConnected)
        {
            Serial.println("Cannot send data: Peer not connected");
            return false;
        }

        struct_message messageToSend = msg;
        messageToSend.messageID = millis();

        SemaphoreHandle_t ackSemaphore = xSemaphoreCreateBinary();
        if (ackSemaphore == nullptr)
        {
            Serial.println("Failed to create semaphore for ack");
            return false;
        }

        xSemaphoreTake(mapMutex, portMAX_DELAY);
        ackSemaphores[messageToSend.messageID] = ackSemaphore;
        xSemaphoreGive(mapMutex);

        Serial.print("Sending message ID: ");
        Serial.println(messageToSend.messageID);
        Serial.print("messageType: ");
        Serial.println(messageToSend.messageType);
        Serial.print("command: ");
        Serial.println(messageToSend.command);
        Serial.print("data: ");
        Serial.println(messageToSend.data);

        bool ackReceived = false;
        while (!ackReceived)
        {
            if (xQueueSend(outgoingQueue, &messageToSend, pdMS_TO_TICKS(100)) != pdPASS)
            {
                Serial.println("Failed to queue outgoing message");
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }
            else
            {
                Serial.print("Message queued: ID ");
                Serial.println(messageToSend.messageID);
            }

            if (xSemaphoreTake(ackSemaphore, pdMS_TO_TICKS(timeout_ms)) == pdTRUE)
            {
                Serial.print("ACK received for message ID: ");
                Serial.println(messageToSend.messageID);
                ackReceived = true;
            }
            else
            {
                Serial.print("ACK not received for message ID: ");
                Serial.println(messageToSend.messageID);
                vTaskDelay(pdMS_TO_TICKS(500));
                Serial.println("Retrying to send message...");
            }
        }

        xSemaphoreTake(mapMutex, portMAX_DELAY);
        ackSemaphores.erase(messageToSend.messageID);
        xSemaphoreGive(mapMutex);
        vSemaphoreDelete(ackSemaphore);

        return true;
    }

    bool ConnectWithRemote::isPeerConnected()
    {
        return peerConnected;
    }

    ConnectWithRemote *ConnectWithRemote::getActiveInstance()
    {
        return activeInstance;
    }

    void ConnectWithRemote::onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (activeInstance)
        {
            activeInstance->handleOnDataSent(mac_addr, status);
        }
    }

    void ConnectWithRemote::onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        if (activeInstance)
        {
            activeInstance->handleOnDataRecv(mac_addr, incomingData, len);
        }
    }

    void ConnectWithRemote::handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (status == ESP_NOW_SEND_SUCCESS)
        {
            Serial.println("Data sent successfully");
        }
        else
        {
            Serial.println("Data send failed");
        }
    }

    void ConnectWithRemote::handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        Serial.println("onDataRecvCallback invoked in sender");
        Serial.print("Data length: ");
        Serial.println(len);

        if (len >= sizeof(struct_message))
        {
            struct_message receivedMessage;
            memcpy(&receivedMessage, incomingData, sizeof(struct_message));

            receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
            receivedMessage.command[sizeof(receivedMessage.command) - 1] = '\0';
            receivedMessage.data[sizeof(receivedMessage.data) - 1] = '\0';

            Serial.print("Received messageID: ");
            Serial.println(receivedMessage.messageID);
            Serial.print("messageType: ");
            Serial.println(receivedMessage.messageType);
            Serial.print("command: ");
            Serial.println(receivedMessage.command);
            Serial.print("data: ");
            Serial.println(receivedMessage.data);

            if (!peerConnected)
            {
                memcpy(peerMAC, mac_addr, 6);
                addPeer(mac_addr);
            }

            if (strcmp(receivedMessage.messageType, "ack") == 0)
            {
                Serial.print("Received ACK for message ID: ");
                Serial.println(receivedMessage.messageID);

                xSemaphoreTake(mapMutex, portMAX_DELAY);
                auto it = ackSemaphores.find(receivedMessage.messageID);
                if (it != ackSemaphores.end())
                {
                    xSemaphoreGive(it->second);
                }
                xSemaphoreGive(mapMutex);
            }
            else if (strcmp(receivedMessage.messageType, "command") == 0)
            {
                Serial.println("Received command message");
                HandleEvents::getInstance().processCommand(receivedMessage.command, receivedMessage.data);

                if (strcmp(receivedMessage.command, "Boop") == 0)
                {
                    shouldStartMainMenu = true;
                    xQueueReset(outgoingQueue);
                    Serial.println("Set shouldStartMainMenu to true");
                }

                struct_message ackMessage;
                ackMessage.messageID = receivedMessage.messageID;
                strcpy(ackMessage.messageType, "ack");
                ackMessage.command[0] = '\0';
                ackMessage.data[0] = '\0';

                esp_err_t result = esp_now_send(mac_addr, reinterpret_cast<uint8_t *>(&ackMessage), sizeof(ackMessage));
                if (result != ESP_OK)
                {
                    Serial.println("Failed to send ACK");
                }
                else
                {
                    Serial.println("ACK sent to receiver");
                }
            }
            else
            {
                Serial.println("Received unknown message type");
            }
        }
        else
        {
            Serial.println("Received message with incorrect size");
        }
    }

    void ConnectWithRemote::addPeer(const uint8_t *mac_addr)
    {
        if (!esp_now_is_peer_exist(mac_addr))
        {
            esp_now_peer_info_t peerInfo = {};
            memcpy(peerInfo.peer_addr, mac_addr, 6);
            peerInfo.channel = 1;
            peerInfo.encrypt = false;

            if (esp_now_add_peer(&peerInfo) == ESP_OK)
            {
                Serial.println("Peer added successfully in sender");
                peerConnected = true;
            }
            else
            {
                Serial.println("Failed to add peer");
            }
        }
        else
        {
            Serial.println("Peer already exists");
            peerConnected = true;
        }
    }

    void ConnectWithRemote::processOutgoingQueueTask(void *pvParameters)
    {
        ConnectWithRemote *instance = static_cast<ConnectWithRemote *>(pvParameters);
        struct_message message;

        for (;;)
        {
            if (xQueueReceive(instance->outgoingQueue, &message, portMAX_DELAY) == pdPASS)
            {
                esp_err_t result = esp_now_send(instance->peerMAC, reinterpret_cast<uint8_t *>(&message), sizeof(message));
                if (result != ESP_OK)
                {
                    Serial.print("Failed to send data via ESP-NOW, error code: ");
                    Serial.println(result);
                }
                else
                {
                    Serial.print("Message sent: ID ");
                    Serial.println(message.messageID);
                }
            }
        }
    }

} // namespace NuggetsInc
