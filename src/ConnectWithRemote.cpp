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
            return;
        }

        if (esp_now_init() != ESP_OK)
        {
            int retries = 3;
            while (esp_now_init() != ESP_OK && retries > 0)
            {
                delay(1000);
                retries--;
            }

            if (retries == 0)
            {
                return;
            }
        }

        esp_now_register_send_cb(onDataSentCallback);
        esp_now_register_recv_cb(onDataRecvCallback);
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
            }
        }
    }

    void ConnectWithRemote::sendData(const uint8_t *data, size_t len)
    {
        if (!peerConnected)
        {
            return;
        }

        if (len > 250)
        {
            return;
        }

        esp_now_send(peerMAC, data, len);
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
        if (status != ESP_NOW_SEND_SUCCESS)
        {
        }
    }

    void ConnectWithRemote::handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
    {
        if (!peerConnected)
        {
            memcpy(peerMAC, mac_addr, 6);
            addPeer(mac_addr);
            peerConnected = true;
        }

        if (len >= sizeof(HandleEvents::struct_message))
        {
            HandleEvents::struct_message receivedMessage;
            memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

            receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
            receivedMessage.command[sizeof(receivedMessage.command) - 1] = '\0';

            if (strcmp(receivedMessage.messageType, "command") == 0)
            {
                HandleEvents::getInstance().processCommand(receivedMessage.command);
            }
            else if (strcmp(receivedMessage.messageType, "response") == 0)
            {
                // Handle response
            }
        }
        else
        {
        }
    }
}
