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

        /**
         * @brief Initializes ESP-NOW and registers callbacks.
         */
        void begin();

        /**
         * @brief Sends data to the connected peer.
         * 
         * @param data Pointer to the data buffer.
         * @param len Length of the data in bytes.
         */
        void sendData(const uint8_t *data, size_t len);

        /**
         * @brief Checks if a peer is connected.
         * 
         * @return true If a peer is connected.
         * @return false Otherwise.
         */
        bool isPeerConnected() const { return peerConnected; }

    private:
        uint8_t peerMAC[6];          // MAC address of the peer device
        bool peerConnected;          // Flag indicating if a peer is connected

        // Static callbacks required by ESP-NOW
        static void onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
        static void onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        // Instance methods to handle callbacks
        void handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
        void handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

        // Pointer to the active instance for callback delegation
        static ConnectWithRemote *activeInstance;
    };
}

#endif // CONNECT_WITH_REMOTE_H
