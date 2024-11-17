#ifndef CONNECT_WITH_REMOTE_H
#define CONNECT_WITH_REMOTE_H

#include <esp_now.h>
#include <WiFi.h>        // Ensure WiFi.h is included
#include <Arduino.h>

namespace NuggetsInc
{
    class HandleEvents;

    class ConnectWithRemote
    {
    public:
        ConnectWithRemote();
        ~ConnectWithRemote();

        void begin();
        void sendData(const uint8_t *data, size_t len);
        bool isPeerConnected() const { return peerConnected; }

        // Public static method to get the active instance
        static ConnectWithRemote* getActiveInstance();

    private:
        uint8_t peerMAC[6];
        bool peerConnected;

        // Callback functions
        static void onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
        static void onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        // Handle callbacks
        void handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
        void handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        // Add a peer
        void addPeer(const uint8_t *mac_addr);

        // Static pointer to the active instance
        static ConnectWithRemote *activeInstance;
    };
}

#endif // CONNECT_WITH_REMOTE_H
