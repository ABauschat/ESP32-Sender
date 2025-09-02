#include "Router.h"

namespace NuggetsInc {

String Router::macToString(const uint8_t mac[6]) {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}

bool Router::stringToMac(const String& s, uint8_t out[6]) {
    int b[6];
    if (sscanf(s.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
               &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) == 6) {
        for (int i = 0; i < 6; i++) {
            out[i] = (uint8_t)b[i];
        }
        return true;
    }
    return false;
}

} // namespace NuggetsInc
