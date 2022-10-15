#include "ItemDHT.h"
#include "Logger.h"

#define NODEBUG_PRINT
#include "debug_print.h"

ItemDHT::ItemDHT(const char *id, DHT_Unified *dht) : Item(id)
{
    this->dht = dht;
    homie.advertise("temp").setDatatype("float").setRetained(true);
    homie.advertise("humd").setDatatype("float").setRetained(true);
}

void ItemDHT::read()
{
    sensors_event_t event;
    String v = "NaN";
    Logger &logger = Logger::getInstance();

    dht->temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        logger.logf_P(LOG_ERR, PSTR("Error reading temperature"));
    }
    else
    {

        logger.logf_P(LOG_INFO, PSTR("Temperature: %.1f °C"), event.temperature);
        v = String(event.temperature);
        homie.setProperty("temp").send(v);
    }

    dht->humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        logger.logf_P(LOG_ERR, PSTR("Error reading humidity"));
    }
    else
    {
        logger.logf_P(LOG_INFO, PSTR("Humidity: %.1f %%\n"), event.relative_humidity);
        v = String(event.relative_humidity);
        homie.setProperty("humd").send(v);
    }
}

void ItemDHT::loop()
{
    if (millis() - dhtTimer > 60000)
    {
        dhtTimer = millis();
        read();
    }
}