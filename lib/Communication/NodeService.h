#ifndef NODE_SERVICE_H
#define NODE_SERVICE_H

#include <Arduino.h>
#include "Node.h"
#include "MessageTypes.h"

namespace NuggetsInc {

// A small façade around Node that provides clearer, task-oriented send APIs
// without changing the existing Node behavior. This is step 1 to gradually
// split responsibilities while keeping risk low.
class NodeService {
public:
    explicit NodeService(Node* node) : node_(node) {}

    // Convenience: build a command message and send it blocking with ACK
    // Defaults preserve current DisplayUtils policy (100ms, up to 2 retries)
    bool sendCommandBlocking(uint8_t commandID,
                             const char* data = nullptr,
                             uint32_t ackTimeoutMs = 100,
                             int maxRetries = 2,
                             Node::RouteMode routeMode = Node::RouteMode::AUTO,
                             bool frameFirst = false);

    // Convenience: build a command message and enqueue it (no ACK wait)
    bool sendCommandNonBlocking(uint8_t commandID,
                                const char* data = nullptr);

    // Power-user: send a fully-formed message via Node's blocking path
    bool sendRawBlocking(const struct_message& msg,
                         uint32_t ackTimeoutMs,
                         int maxRetries,
                         Node::RouteMode routeMode = Node::RouteMode::AUTO,
                         bool frameFirst = false);

    // Common pattern convenience methods
    bool sendBoop(const char* macData = nullptr);
    bool sendSync(const char* macData = nullptr);
    bool sendAck(uint32_t originalMessageID, const char* originalPath = nullptr);

    // Pass-through helpers (simplified)
    inline String lastRouteInfo() const { return String("DIRECT"); }
    inline void setRouteMode(Node::RouteMode m) { /* no-op - always direct */ }

private:
    static void buildCommandMessage(struct_message& out,
                                    uint8_t commandID,
                                    const char* data);
    static void buildAckMessage(struct_message& out,
                                uint32_t originalMessageID,
                                const char* originalPath);
    static void buildProbeMessage(struct_message& out,
                                  uint32_t messageID);

    Node* node_;
};

} // namespace NuggetsInc

#endif // NODE_SERVICE_H

