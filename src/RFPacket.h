#pragma once

#include <Arduino.h>

enum RFDeviceType: uint8_t {
    TEMPERATURE,
    CONTACT
};

enum RFPacketType: uint8_t {
    PAIRING,
    DATA
};

using RFPayload = unsigned char[22];

// when changing RFPacket structure be aware of potential padding bytes
// as it does not have to be the same on all platforms
struct RFSensorPacket {
    uint32_t seqno;
    uint32_t srcAdr;
    RFDeviceType deviceType;
    uint8_t vcc;
    RFPayload payload;
};

struct RFActuatorPacket {
    uint32_t seqno;
    uint32_t dstAdr;
    RFPacketType pktType;
    uint8_t payload[23];
};