#pragma once

#include <Syslog.h>
#include <WiFiUdp.h>
#include <Arduino.h>

class Logger
{
    public:
        static Logger& getInstance();
    private:
        uint8_t _logMask = 0;
        
        // syslog related
        WiFiUDP* _udpClient = nullptr;
        Syslog* _syslog = nullptr;
        const char* _syslogHost;
        uint16_t _syslogPort;
        const char* _syslogAppName;
        const char* _syslogDeviceName;

        // serial related
        HardwareSerial* _serial = nullptr;

        Logger(){};

        char* formatMessage_P(PGM_P fmt_P, va_list arg);
        char* formatMessage(const char* fmt, va_list arg);

    public:
        Logger(Logger const&)          = delete;
        void operator=(Logger const&)  = delete;
        void enableSyslog(const char* deviceName, const char* appName, const char* syslogHost, uint16_t syslogPort=514);
        void enableSerial(HardwareSerial& serial);
        void setLogLevel(uint8_t level);
        bool logf(uint16_t pri, const char *fmt, ...);
        bool logf(const char *fmt, ...);
        bool logf_P(uint16_t pri, PGM_P fmt_P, ...);
        
};