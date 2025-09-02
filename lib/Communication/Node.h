// Node.h
#ifndef CONNECT_WITH_REMOTE_H
#define CONNECT_WITH_REMOTE_H

#include <esp_now.h>
#include <WiFi.h>
#include <map>
#include <vector>
#include <set>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "MessageTypes.h"
#include "Utils/TimeUtils.h"

namespace NuggetsInc {

class HandleEvents;
class Router;
class MessageHandler;

class Node
{
public:
    enum class RouteMode { AUTO, DIRECT, ROUTED }; 

    Node();
    ~Node();

    void begin();
    bool sendDataBlocking(const struct_message &msg, uint32_t timeout_ms = 2000, int maxRetries = 3);
    bool sendDataNonBlocking(const struct_message &msg);
    bool isPeerIntialized();
    static Node* getActiveInstance();

    void setRouteMode(RouteMode mode) { routeMode_ = mode; }
    String getLastRouteInfo() const { 
        switch (routeMode_) {
            case RouteMode::AUTO: return "AUTO";
            case RouteMode::DIRECT: return "DIRECT";
            case RouteMode::ROUTED: return "ROUTED";
        }
        return "UNKNOWN"; }

    // ACK notification from MessageHandler
    void notifyAckReceived(uint32_t messageID);

    volatile bool InitializeMenu;

private:
    uint8_t peerMAC[6];
    bool peerIntialized;
    static void onDataSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void onDataRecvCallback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
    void addPeer(const uint8_t *mac_addr);

    // Delegated components
    Router* router_;
    RouteMode routeMode_ = RouteMode::AUTO;
    MessageHandler* messageHandler_;


    void handleOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    void handleOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

    static Node* activeInstance;

    QueueHandle_t outgoingQueue;
    std::map<uint32_t, SemaphoreHandle_t> ackSemaphores;
    SemaphoreHandle_t mapMutex;

    // Helper functions
    void setSelfMac(uint8_t out[6]);
    void prepareMessage(struct_message &msg, bool isBlocking);

    static void processOutgoingQueueTask(void *pvParameters);
};

} // namespace NuggetsInc

#endif // CONNECT_WITH_REMOTE_H
