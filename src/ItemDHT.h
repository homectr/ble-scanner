#pragma once

#include "Item.h"
#include "DHT_U.h"
#include <Homie.h>

class ItemDHT : public Item
{

protected:
    DHT_Unified *dht;
    HomieNode homie = HomieNode("dht", "DHT", "dht");
    unsigned long dhtTimer = 0;

public:
    ItemDHT(const char *ide, DHT_Unified *dht);
    void read();
    void loop() override;
};