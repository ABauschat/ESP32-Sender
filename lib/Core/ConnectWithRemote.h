// ConnectWithRemote.h
#ifndef CONNECT_WITH_REMOTE_H
#define CONNECT_WITH_REMOTE_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <map>
#include "MessageTypes.h"

namespace NuggetsInc {

class HandleEvents; 

class ConnectWithRemote
{
public:
    ConnectWithRemote();
    ~ConnectWithRemote();

    void begin();
    bool sendDataBlocking(const struct_message &msg, uint32_t timeout_ms = 2000);
    bool isPeerConnected();
    static ConnectWithRemote* getActiveInstance();

    volatile bool shouldStartMainMenu; 

private:
    uint8_t peerMAC[6];
    bool peerConnected;

    static void onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

    void handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    void handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

    void addPeer(const uint8_t *mac_addr);
    static ConnectWithRemote* activeInstance;

    QueueHandle_t outgoingQueue;

    std::map<uint32_t, SemaphoreHandle_t> ackSemaphores;
    SemaphoreHandle_t mapMutex;

    static void processOutgoingQueueTask(void *pvParameters);
};

} // namespace NuggetsInc

#endif // CONNECT_WITH_REMOTE_H
