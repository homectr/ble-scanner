#pragma once

#include <Homie.h>
#include "RFPacket.h"
#include "Logger.h"

#define DEVICE_STR_SENSOR_TEMP "temp"
#define DEVICE_STR_SENSOR_CONTACT "cont"
#define DEVICE_STR_SENSOR_HUMIDITY "humd"

class RFDevice
{

public:
    RFSensorType type;
    uint32_t id;
    char *idStr = nullptr;
    uint32_t seqno;
    HomieNode *homie = nullptr;
    Logger &_logger = Logger::getInstance();

public:
    RFDevice(RFSensorType type, uint32_t id, HomieNode *homie);
    virtual ~RFDevice();
    virtual void update(RFSensorPayload &payload){};
    virtual void cmd(RFSensorPayload &payload){};
};

class RFSensorTemp : public RFDevice
{
public:
    float temp = 0;

public:
    RFSensorTemp(uint32_t id, HomieNode *homie);
    virtual ~RFSensorTemp(){};
    virtual void update(RFSensorPayload &payload) override;
};

class RFSensorContact : public RFDevice
{
public:
    bool open = false;

public:
    RFSensorContact(uint32_t id, HomieNode *homie);
    virtual ~RFSensorContact(){};
    virtual void update(RFSensorPayload &payload) override;
};

class RFSensorHumidity : public RFDevice
{
public:
    float hum = 0;

public:
    RFSensorHumidity(uint32_t id, HomieNode *homie);
    virtual ~RFSensorHumidity(){};
    virtual void update(RFSensorPayload &payload) override;
};