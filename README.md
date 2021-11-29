# nRF24 to MQTT Bridge
Homie based nRF24 to MQTT bridge.
1. Waits for packets from nRF24 sensors and forwards them to configured mqtt. See https://github.com/homectr/nrf24-sensor-node
2. forwards data to nRF24 actuators (to be added)

## Configuration
This project uses ESP Homie framework https://github.com/homieiot/homie-esp8266, you can find detailed information about possible configuration there.
On top of standard configuration this project requires few addition settings in Homie configuration file to be present.
* syslogHost - string containing host name or an ip address of your syslog server
* syslogPort - optional syslog port number (default is 514)
* logLevel - optional logging level (default is 5)

### Logging levels
* 0 - system is unusable
* 1 - action must be taken immediately
* 2 - critical conditions
* 3 - error conditions
* 4 - warning conditions
* 5 - normal but significant condition
* 6 - informational
* 7 - debug-level messages

### Uploading configuration
Set device to configuration mode (hold SET button while powering up device). Without any configuration device starts in configuration mode automatically.
Follow instructions from https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/http-json-api/

### Example Homie config
See https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/json-configuration-file/ for full documentation about Homie configuration

```
{
    "name": "RF24Bridge",
    "device_id": "rf24br",
    "device_stats_interval": 300,
    "wifi": {
      "ssid": "your_wifi_ssid",
      "password": "your_wifi_password"
    },
    "mqtt": {
      "host": "your_mqtt_hostname_or_ip",
      "port": 1883
    },
    "ota": {
      "enabled": true
    },
    "settings": {
      "syslogHost": "your_syslog_hostname_or_ip",
      "syslogPort": 514,
      "logLevel": 5
    }
  }
```
