#include "ItemDHT.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

ItemDHT::ItemDHT(const char* id, DHT_Unified* dht):Item(id){
    this->dht = dht;
    homie.advertise("temp").setDatatype("float").setRetained(true);
    homie.advertise("humd").setDatatype("float").setRetained(true);
}

void ItemDHT::read(){
    sensors_event_t event;
    String v = "NaN";

    dht->temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        DEBUG_PRINT(PSTR("Error reading temperature!"));
    }
    else {
        DEBUG_PRINT(PSTR("  %s Temperature: %.1f °C\n"),getId(), event.temperature);
        v = String(event.temperature);
        homie.setProperty("temp").send(v);
    }  

    dht->humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        DEBUG_PRINT(PSTR("Error reading humidity!"));
    }
    else {
        DEBUG_PRINT(PSTR("  %s Humidity: %.1f %%\n"),getId(), event.relative_humidity);
        v = String(event.relative_humidity);
        homie.setProperty("humd").send(v);
    }

}

void ItemDHT::loop(){
    if (millis()-dhtTimer > 15000){
        dhtTimer = millis();
        read();
    }

}