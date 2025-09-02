#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>

namespace NuggetsInc {

// Safe timestamp handling that works across millis() rollover (~49.7 days)
using msec32 = uint32_t;

// Get current time as 32-bit unsigned (safe for rollover calculations)
static inline msec32 now_ms() { 
    return (msec32)millis(); 
}

// Check if a timeout has elapsed (rollover-safe)
static inline bool has_elapsed(msec32 start_ms, msec32 timeout_ms) {
    return (msec32)(now_ms() - start_ms) >= timeout_ms;
}

// Get elapsed time since start (rollover-safe)
static inline msec32 elapsed_ms(msec32 start_ms) {
    return (msec32)(now_ms() - start_ms);
}

// Check if timestamp is within a window (rollover-safe)
static inline bool within_window(msec32 timestamp, msec32 window_ms) {
    return elapsed_ms(timestamp) < window_ms;
}

} // namespace NuggetsInc

#endif // TIME_UTILS_H
