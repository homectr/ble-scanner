#pragma once

#include <Homie.h>
#include "RFPacket.h"

class RFDevice {

    public:
        RFSensorType type;
        uint32_t id;
        const char* idStr = nullptr;
        uint32_t seqno;
        HomieNode *homie = nullptr;

    public:
        RFDevice(RFSensorType type, uint32_t id, HomieNode *homie);
        virtual void update(RFSensorPayload& payload){};
        virtual void cmd(RFSensorPayload& payload){};

};

class RFSensorTemp : public RFDevice {
    public:
        float temp = 0;

    public:
        RFSensorTemp(uint32_t id, HomieNode *homie);
        void update(RFSensorPayload& payload) override;
};

class RFSensorContact : public RFDevice {
    public:
        bool open = false;
    public:
        RFSensorContact(uint32_t id, HomieNode *homie);
        void update(RFSensorPayload& payload) override;
};

class RFSensorHumidity : public RFDevice {
    public:
        float hum = 0;
    public:
        RFSensorHumidity(uint32_t id, HomieNode *homie);
        void update(RFSensorPayload& payload) override;
};