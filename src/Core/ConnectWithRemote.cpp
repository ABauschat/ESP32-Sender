#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include <esp_wifi.h> 
#include "Application.h"
#include "StateFactory.h"


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
            Serial.println("Cannot send data: Payload too large");
            return;
        }

        esp_now_send(peerMAC, data, len);
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
        if (len >= sizeof(HandleEvents::struct_message))
        {
            HandleEvents::struct_message receivedMessage;
            memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

            receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
            receivedMessage.command[sizeof(receivedMessage.command) - 1] = '\0';
            receivedMessage.data[sizeof(receivedMessage.data) - 1] = '\0';

            if (strcmp(receivedMessage.messageType, "command") == 0)
            {
                HandleEvents::getInstance().processCommand(receivedMessage.command, receivedMessage.data);
                Serial.println("Received command message");
            }
            else
            {
                Serial.println("Received unknown message type");
            }
        }


        if (!peerConnected)
        {
            memcpy(peerMAC, mac_addr, 6);
            addPeer(mac_addr);
            
            Application& app = Application::getInstance();
            app.changeState(StateFactory::createState(MENU_STATE));
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
                Serial.println("Peer added successfully");
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

      bool ConnectWithRemote::isPeerConnected() {
        return peerConnected;
      }
}
