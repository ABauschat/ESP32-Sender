#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <Arduino.h>
#include <map>
#include <set>
#include "MessageTypes.h"
#include "Utils/TimeUtils.h"

namespace NuggetsInc {

class Router;
class NodeService;
class Node;

// Extracted message processing logic from Node
class MessageHandler {
public:
    MessageHandler(Router* router, NodeService* nodeService, Node* node = nullptr);

    // Message processing
    void processReceivedMessage(const uint8_t* senderMac, const struct_message& msg);
    
    // ACK handling
    void processAckMessage(const uint8_t* senderMac, const struct_message& ackMsg);
    
    // Command message handling
    void processCommandMessage(const uint8_t* senderMac, const struct_message& cmdMsg);
    
    // Deduplication
    bool isDuplicateMessage(const uint8_t src[6], uint32_t messageID);

private:
    Router* router_;
    NodeService* nodeService_;
    Node* node_;

    // Deduplication cache
    struct MsgKey { 
        uint8_t mac[6]; 
        uint32_t id; 
    };
    struct MsgKeyCmp { 
        bool operator()(const MsgKey& a, const MsgKey& b) const {
            int c = memcmp(a.mac, b.mac, 6); 
            return c < 0 || (c == 0 && a.id < b.id);
        }
    };
    std::map<MsgKey, msec32, MsgKeyCmp> recentMsgCache_;

    // Helper functions
    void setSelfMac(uint8_t out[6]);
    bool isZeroMac(const uint8_t mac[6]);
    String macToString(const uint8_t mac[6]);
    bool stringToMac(const String& s, uint8_t out[6]);
    bool isDestinationForSelf(const struct_message& msg);
};

} // namespace NuggetsInc

#endif // MESSAGE_HANDLER_H
