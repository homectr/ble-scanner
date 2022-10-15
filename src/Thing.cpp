#include "Thing.h"
#include <Arduino.h>
#include "utils.h"
#include "handlers.h"
#include "RF24Bridge.h"
#include <DHT_U.h>
#include "ItemDHT.h"
#include <Homie.h>
#include "Logger.h"

#define REBOOT_TIMEOUT 5000

#define NODEBUG_PRINT
#include "debug_print.h"

#define NRF_CSNPIN 15 // D8 : use HW CS pin + 3-5kOhm resistor to connect it to GND
#define NRF_CEPIN 2   // D4

#define DHT_PIN 5 // D1

Thing::Thing()
{
    // create properties for device

    // use homie/{device_id}/thing/cmd/set to send commands to the thing
    homie.advertise("cmd").setDatatype("string").settable(globalCmdHandler);

    Item *item;
    // create items
    item = new RF24Bridge("rf24brg", NRF_CEPIN, NRF_CSNPIN);
    items.add(item);

    // create DHT sensor
    DHT_Unified *dht = new DHT_Unified(DHT_PIN, DHT22);
    dht->begin();
    item = new ItemDHT("dht", dht);
    items.add(item);

    DEBUG_PRINT(PSTR("[Thing:Thing] Thing created\n"));
}

void Thing::setup()
{
    DEBUG_PRINT(PSTR("[Thing:Setup] SETUP\n"));

    if (!Homie.isConfigured())
    {
        CONSOLE(PSTR("Homie not configured. Skipping Thing setup. Loop will be ignored.\n"));
        return;
    }

    DEBUG_PRINT(PSTR("[Thing:Setup] Completed\n"));

    configured = true;
}

bool Thing::updateHandler(const HomieNode &node, const String &property, const String &value)
{
    // call all update handlers for all items until one returns true
    ListEntry<Item> *i = items.getList();
    bool updated = false;
    while (i && !updated)
    {
        updated = i->entry->updateHandler(property, value);
        i = i->next;
    }
    return updated;
}

bool Thing::cmdHandler(const String &value)
{
    ListEntry<Item> *i = items.getList();
    bool updated = false;

    if (value == "reboot")
    {
        Logger::getInstance().logf_P(LOG_NOTICE, PSTR("Reboot requested"), value.c_str());
        rebootTimer = millis();
        updated = true;
    }

    while (i && !updated)
    {
        updated = i->entry->cmdHandler(value);
        i = i->next;
    }
    return updated;
}

void Thing::loop()
{
#ifndef NODEBUG_PRINT
    if (millis() - aliveTimer > 15000)
    {
        aliveTimer = millis();
        DEBUG_PRINT(PSTR("[Thing] alive ms=%lu\n"), millis());
        if (!isConfigured())
            DEBUG_PRINT(PSTR("[Thing] not configured. Skipping loop.\n"));
    }
#endif

    if (!isConfigured())
        return;

    // check if any item requested reboot
    ListEntry<Item> *i = items.getList();
    while (i)
    {
        if (i->entry->rebootNeeded && rebootTimer == 0)
            rebootTimer = millis();
        i->entry->loop();
        i = i->next;
    }

    // can't use delay in Homie callback, so we need to handle delay before reboot here
    if (rebootTimer > 0 && millis() - rebootTimer > REBOOT_TIMEOUT)
    {
        rebootTimer = 0;
        Homie.reboot();
    }
}