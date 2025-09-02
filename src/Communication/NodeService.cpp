#include "NodeService.h"
#include "MessageTypes.h"
#include <cstring>
#include <WiFi.h>

namespace NuggetsInc {

void NodeService::buildCommandMessage(struct_message& out,
                                      uint8_t commandID,
                                      const char* data) {
    memset(&out, 0, sizeof(out));
    strcpy(out.messageType, "cmd");
    out.commandID = commandID;
    if (data) {
        strncpy(out.data, data, sizeof(out.data)-1);
        out.data[sizeof(out.data)-1] = '\0';
    } else {
        out.data[0] = '\0';
    }
}

bool NodeService::sendCommandBlocking(uint8_t commandID,
                                      const char* data,
                                      uint32_t ackTimeoutMs,
                                      int maxRetries,
                                      Node::RouteMode routeMode,
                                      bool frameFirst) {
    if (!node_) return false;
    struct_message m;
    buildCommandMessage(m, commandID, data);
    return node_->sendDataBlocking(m, ackTimeoutMs, maxRetries);
}

bool NodeService::sendCommandNonBlocking(uint8_t commandID,
                                         const char* data) {
    if (!node_) return false;
    struct_message m; buildCommandMessage(m, commandID, data);
    return node_->sendDataNonBlocking(m);
}

bool NodeService::sendRawBlocking(const struct_message& msg,
                                  uint32_t ackTimeoutMs,
                                  int maxRetries,
                                  Node::RouteMode routeMode,
                                  bool frameFirst) {
    if (!node_) return false;
    return node_->sendDataBlocking(msg, ackTimeoutMs, maxRetries);
}

void NodeService::buildAckMessage(struct_message& out,
                                  uint32_t originalMessageID,
                                  const char* originalPath) {
    memset(&out, 0, sizeof(out));
    out.messageID = originalMessageID;
    strcpy(out.messageType, "ack");
    out.commandID = 0;
    out.data[0] = '\0';
    out.path[0] = '\0'; // No path needed for direct communication
}

bool NodeService::sendBoop(const char* macData) {
    return sendCommandBlocking(CMD_BOOOP, macData);
}

bool NodeService::sendSync(const char* macData) {
    return sendCommandBlocking(CMD_SYNC_NODES, macData);
}

bool NodeService::sendAck(uint32_t originalMessageID, const char* originalPath) {
    if (!node_) return false;
    struct_message ack;
    buildAckMessage(ack, originalMessageID, originalPath);
    return node_->sendDataNonBlocking(ack);
}

} // namespace NuggetsInc

