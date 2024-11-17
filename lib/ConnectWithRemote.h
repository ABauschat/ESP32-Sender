//Device Sender 
#ifndef CONNECT_WITH_REMOTE_H
#define CONNECT_WITH_REMOTE_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

namespace NuggetsInc
{
    class HandleEvents; // Forward declaration

    class ConnectWithRemote
    {
    public:
        ConnectWithRemote();
        ~ConnectWithRemote();

        void begin();
        void sendData(const uint8_t *data, size_t len);
        bool isPeerConnected() const { return peerConnected; }

    private:
        uint8_t peerMAC[6];          
        bool peerConnected;         

        static void onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
        static void onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        void handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
        void handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        static ConnectWithRemote *activeInstance;
    };
}

#endif // CONNECT_WITH_REMOTE_H
