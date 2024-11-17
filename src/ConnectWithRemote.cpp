#include "ConnectWithRemote.h"
#include "HandleEvents.h"

namespace NuggetsInc
{
    ConnectWithRemote* ConnectWithRemote::activeInstance = nullptr;

    ConnectWithRemote::ConnectWithRemote()
        : peerConnected(false)
    {
        // Set All to all FF
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

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("ESP-NOW initialization failed");
            return;
        }

        esp_now_register_send_cb(onDataSentCallback);
        esp_now_register_recv_cb(onDataRecvCallback);

        Serial.println("ESP-NOW initialized successfully");
    }

    void ConnectWithRemote::sendData(const uint8_t *data, size_t len)
    {
        if (!peerConnected)
        {
            Serial.println("Cannot send data: Peer not connected");
            return;
        }

        esp_err_t result = esp_now_send(peerMAC, data, len);
        if (result == ESP_OK)
        {
            Serial.println("Data sent successfully");
        }
        else
        {
            Serial.println("Failed to send data");
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
            // Assume the first received message is for handshake
            memcpy(peerMAC, mac_addr, 6);
            peerConnected = true;
            Serial.println("Peer connected successfully");

            // Optionally, send a handshake acknowledgment
            // Example:
            // struct_message ack;
            // strcpy(ack.messageType, "ack");
            // strcpy(ack.command, "connected");
            // sendData((uint8_t*)&ack, sizeof(ack));
        }

        if (len >= sizeof(HandleEvents::struct_message))
        {
            HandleEvents::struct_message receivedMessage;
            memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

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
