
#include <Homie.h>
#include "Thing.h"
#include "handlers.h"
#include "Logger.h"

#define NODEBUG_PRINT
#include "debug_print.h"

#define HOMIE_LED_PIN       4 // D2
#define FIRMWARE_NAME       "rf24bridge"
#define FIRMWARE_VERSION    "1.0.0"

#define ALIVE_TIMER         15000  // used to print "alive" messages in debug mode

Thing* thing = NULL;

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    SPIFFS.begin();

    Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
    Homie.setGlobalInputHandler(globalUpdateHandler);
    Homie.setLedPin(HOMIE_LED_PIN, 1);
    Homie.onEvent(globalOnHomieEvent);

    // configure logger
    HomieSetting<const char*> syslogHost("syslogHost","Syslog hostname or ip address");
    HomieSetting<long> syslogPort("syslogPort","Syslog port");
    HomieSetting<long> logLevel("logLevel","Log level");
    syslogHost.setDefaultValue("");
    syslogPort.setDefaultValue(514);
    logLevel.setDefaultValue(LOG_DEBUG);

    // create The Thing
    // has to be created before Homie.setup() call
    thing = new Thing();

    Homie.setup();

    // call device setup only after Homie setup has been called
    thing->setup(); 

    // initialize logger
    Logger& _logger = Logger::getInstance();
    if (strlen(syslogHost.get()) > 0) // if syslog host is specified
        _logger.enableSyslog(Homie.getConfiguration().deviceId, FIRMWARE_NAME, syslogHost.get(), syslogPort.get());
    _logger.enableSerial(Serial);
    _logger.setLogLevel(logLevel.get());

    // this most likely won't be logged as wifi is not connected yet
    bool s = _logger.logf_P(LOG_NOTICE, PSTR("*** Device starting %s v%s"), FIRMWARE_NAME, FIRMWARE_VERSION);

    if (!s) CONSOLE(PSTR("ERROR: Logging not configured properly. host=%s port=%lu\n"),syslogHost.get(), syslogPort.get());
    else CONSOLE(PSTR("Logging up-to level=%d to %s:%lu\n"),logLevel.get(),syslogHost.get(), syslogPort.get());

}

#ifndef NODEBUG_PRINT
unsigned long ms = millis();
#endif

void loop() {
    Homie.loop();
    thing->loop();
    #ifndef NODEBUG_PRINT
    if (millis()-ms > ALIVE_TIMER){
        ms = millis();
        DEBUG_PRINT(PSTR("[main] alive ms=%lu\n"),millis());
    }
    #endif
    
}