#include "MessageHandler.h"
#include "Router.h"
#include "NodeService.h"
#include "Node.h"
#include "HandleEvents.h"
#include "Applications/RelayState.h"
#include "MessageTypes.h"
#include "Utils/TimeUtils.h"
#include <WiFi.h>
#include <esp_now.h>
#include <cstring>

namespace NuggetsInc {
using namespace NuggetsInc; // For TimeUtils functions

MessageHandler::MessageHandler(Router* router, NodeService* nodeService, Node* node)
    : router_(router), nodeService_(nodeService), node_(node) {}
    
void MessageHandler::processReceivedMessage(const uint8_t* senderMac, const struct_message& msg) {
    struct_message receivedMessage = msg;
    receivedMessage.messageType[sizeof(receivedMessage.messageType) - 1] = '\0';
    receivedMessage.data[sizeof(receivedMessage.data) - 1] = '\0';
    receivedMessage.path[sizeof(receivedMessage.path) - 1] = '\0';

    
    if (strcmp(receivedMessage.messageType, "ack") == 0) {
        processAckMessage(senderMac, receivedMessage);
    }
    
    else if (strcmp(receivedMessage.messageType, "cmd") == 0) {
        processCommandMessage(senderMac, receivedMessage);
    }
}

void MessageHandler::processAckMessage(const uint8_t* senderMac, const struct_message& ackMsg) {
    if (node_) {
        node_->notifyAckReceived(ackMsg.messageID);
    }
}

void MessageHandler::processCommandMessage(const uint8_t* senderMac, const struct_message& cmdMsg) {
    if (!isDestinationForSelf(cmdMsg)) {
        return; 
    }
    
    if (isDuplicateMessage(cmdMsg.SenderMac, cmdMsg.messageID)) {
        return;
    }

    nodeService_->sendAck(cmdMsg.messageID, cmdMsg.path);

    if(cmdMsg.commandID != CMD_SYNC_NODES) {
        HandleEvents::getInstance().executeCommand(cmdMsg.commandID, cmdMsg.data);
        return;
    } 

    if (RelayState::getActiveInstance()) {
        RelayState::getActiveInstance()->handleSyncNodes(cmdMsg.data);
    }
}

bool MessageHandler::isDuplicateMessage(const uint8_t src[6], uint32_t messageID) {
    msec32 nowMs = now_ms();
    const msec32 window = 2000;
    MsgKey key;
    memcpy(key.mac, src, 6);
    key.id = messageID;

    auto it = recentMsgCache_.find(key);
    if (it != recentMsgCache_.end() && within_window(it->second, window)) {
        return true;
    }
    recentMsgCache_[key] = nowMs;
    return false;
}

bool MessageHandler::isDestinationForSelf(const struct_message& msg) {
    return isZeroMac(msg.destinationMac) || memcmp(msg.destinationMac, selfMAC_, 6) == 0;
}

void MessageHandler::setSelfMac(uint8_t mac[6]) {
    memcpy(selfMAC_, mac, 6);
}

bool MessageHandler::isZeroMac(const uint8_t mac[6]) {
    for (int i = 0; i < 6; ++i) {
        if (mac[i] != 0) return false;
    }
    return true;
}

String MessageHandler::macToString(const uint8_t mac[6]) {
    return Router::macToString(mac);
}

bool MessageHandler::stringToMac(const String& s, uint8_t out[6]) {
    return Router::stringToMac(s, out);
}

} // namespace NuggetsInc
