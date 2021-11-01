#pragma once

#include <Arduino.h>

enum RFDeviceTypes {
    SENSOR_TEMPERATURE,
    SENSOR_CONTACT
};

using RFPayload = unsigned char[25];

// when changing RFPacket structure be aware of potential padding bytes
// as it does not have to be the same on all platforms
struct RFPacket {
    uint32_t seqno;
    uint16_t deviceId;  // upper byte = id, lower byte = subId
    RFDeviceTypes deviceType;
    RFPayload payload;
};