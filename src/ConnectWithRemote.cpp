#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "esp_wifi.h"

namespace NuggetsInc
{
    ConnectWithRemote *ConnectWithRemote::activeInstance = nullptr;

    ConnectWithRemote::ConnectWithRemote()
        : peerConnected(false)
    {
        memset(peerMAC, 0xFF, sizeof(peerMAC)); 
    }

    ConnectWithRemote::~ConnectWithRemote()
    {
    }

    void ConnectWithRemote::begin()
    {
        activeInstance = this;

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100); 

        if (esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE) != ESP_OK)
        {
            Serial.println("Failed to set WiFi channel");
            return;
        }

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("ESP-NOW initialization failed");
            // Attempt to re-initialize
            int retries = 3;
            while (esp_now_init() != ESP_OK && retries > 0)
            {
                Serial.println("Retrying ESP-NOW initialization...");
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
                Serial.println("Peer added successfully");
            }
            else
            {
                Serial.println("Failed to add peer");
            }
        }
        else
        {
            Serial.println("Peer already exists");
        }
    }

    void ConnectWithRemote::sendData(const uint8_t *data, size_t len)
    {
        if (!peerConnected)
        {
            Serial.println("Cannot send data: Peer not connected");
            return;
        }

        if (len > 250)
        {
            Serial.println("Data size exceeds ESP-NOW limit (250 bytes)");
            return;
        }

        esp_err_t result = esp_now_send(peerMAC, data, len);
        if (result == ESP_OK)
        {
            Serial.println("Data sent successfully");
        }
        else
        {
            Serial.print("Failed to send data. Error code: ");
            Serial.println(result, HEX);

            switch (result)
            {
                case ESP_ERR_ESPNOW_NOT_INIT:
                    Serial.println("Error: ESP-NOW not initialized");
                    break;
                case ESP_ERR_ESPNOW_ARG:
                    Serial.println("Error: Invalid argument");
                    break;
                case ESP_ERR_ESPNOW_INTERNAL:
                    Serial.println("Error: Internal error");
                    break;
                case ESP_ERR_ESPNOW_NO_MEM:
                    Serial.println("Error: Out of memory");
                    break;
                case ESP_ERR_ESPNOW_NOT_FOUND:
                    Serial.println("Error: Peer not found");
                    break;
                case ESP_ERR_ESPNOW_IF:
                    Serial.println("Error: Interface error");
                    break;
                default:
                    Serial.println("Error: Unknown error");
                    break;
            }
        }
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
        String result = (status == ESP_NOW_SEND_SUCCESS) ? "Send success" : "Send failed";
        Serial.println(result);
    }

    void ConnectWithRemote::handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        if (!peerConnected)
        {
            memcpy(peerMAC, mac_addr, 6);
            addPeer(mac_addr); 
            peerConnected = true;
            Serial.println("Peer connected successfully");
        }

        if (len >= sizeof(HandleEvents::struct_message))
        {
            HandleEvents::struct_message receivedMessage;
            memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

            // Ensure null-termination
            receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
            receivedMessage.command[sizeof(receivedMessage.command) - 1] = '\0';

            if (strcmp(receivedMessage.messageType, "command") == 0)
            {
                Serial.print("Received command: ");
                Serial.println(receivedMessage.command);

                HandleEvents::getInstance().processCommand(receivedMessage.command);
            }
            else if (strcmp(receivedMessage.messageType, "response") == 0)
            {
                Serial.print("Received response: ");
                Serial.println(receivedMessage.command);
            }
            else
            {
                Serial.println("Unknown message type received");
            }
        }
        else
        {
            Serial.println("Invalid message received");
        }
    }
}
