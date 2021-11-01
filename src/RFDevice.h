#pragma once

#include <Homie.h>
#include "RFPacket.h"

class RFDevice {

    public:
        RFDeviceTypes type;
        uint16_t id;
        const char* idStr = nullptr;
        uint16_t seqno;
        HomieNode *homie = nullptr;

    public:
        RFDevice(RFDeviceTypes type, uint16_t id);
        virtual void update(RFPayload payload){};
        virtual void cmd(RFPayload payload){};

};

class RFSensorTemp : public RFDevice {
    public:
        float temp = 0;

    public:
        RFSensorTemp(uint16_t id);
        void update(RFPayload payload) override;
};

class RFSensorContact : public RFDevice {
    public:
        bool open = false;
    public:
        RFSensorContact(uint16_t id);
        void update(RFPayload payload) override;
};