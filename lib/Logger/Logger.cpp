#include "Logger.h"

#include <Syslog.h>
#include <WiFiUdp.h>

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::enableSyslog(const char *deviceName, const char *appName, const char *syslogHost, uint16_t syslogPort)
{
    _udpClient = new WiFiUDP();
    _syslog = new Syslog(*_udpClient);
    _syslogAppName = appName;
    _syslogDeviceName = deviceName;
    _syslogHost = syslogHost;
    _syslogPort = syslogPort;
    _syslog->server(syslogHost, syslogPort);
    _syslog->appName(_syslogAppName);
    _syslog->deviceHostname(_syslogDeviceName);
}

void Logger::enableSerial(HardwareSerial &serial)
{
    _serial = &serial;
}

void Logger::setLogLevel(uint8_t level)
{
    _logMask = LOG_UPTO(level);
    if (_syslog)
        _syslog->logMask(_logMask);
}

char *Logger::formatMessage_P(PGM_P fmt_P, va_list args)
{
    char *message = nullptr;
    size_t initialLen;
    size_t len;

    initialLen = strlen_P(fmt_P);
    message = new char[initialLen + 1];

    len = vsnprintf_P(message, initialLen + 1, fmt_P, args);
    if (len > initialLen)
    {
        delete[] message;
        message = new char[len + 1];
        vsnprintf(message, len + 1, fmt_P, args);
    }

    return message;
}

char *Logger::formatMessage(const char *fmt, va_list args)
{
    char *message = nullptr;
    size_t initialLen;
    size_t len;

    initialLen = strlen(fmt);
    message = new char[initialLen + 1];

    len = vsnprintf(message, initialLen + 1, fmt, args);
    if (len > initialLen)
    {
        delete[] message;
        message = new char[len + 1];
        vsnprintf(message, len + 1, fmt, args);
    }

    return message;
}

bool Logger::logf(uint16_t pri, const char *fmt, ...)
{
    va_list args;
    bool result = false;

    // Check priority against logmas values.
    if ((LOG_MASK(LOG_PRI(pri)) & this->_logMask) == 0)
        return true;

    va_start(args, fmt);
    char *msg = formatMessage(fmt, args);

    if (_syslog)
        result = _syslog->log(pri, msg);
    if (_serial)
        _serial->println(msg);
    va_end(args);

    if (msg)
        delete[] msg;
    return result;
}

bool Logger::logf_P(uint16_t pri, PGM_P fmt_P, ...)
{
    va_list args;
    bool result = false;

    // Check priority against logmas values.
    if ((LOG_MASK(LOG_PRI(pri)) & this->_logMask) == 0)
        return true;

    va_start(args, fmt_P);
    char *msg = formatMessage_P(fmt_P, args);
    if (_syslog)
        result = _syslog->log(pri, msg);
    if (_serial)
        _serial->println(msg);
    va_end(args);

    if (msg)
        delete[] msg;
    return result;
}