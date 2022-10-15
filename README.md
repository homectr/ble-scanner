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
1. Set device to configuration mode (hold SET button while powering up device).\
   Without any configuration device starts in configuration mode automatically.
1. Follow instructions from https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/http-json-api/

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

## Using

1. Start rf24 bridge (thi device)
1. Start rf24 sensor node device (or devices) - see project https://github.com/homectr/nrf24-sensor-node.
1. Pair sensor nodes with the bridge. Bridge will automatically create Homie nodes in it configuration for paired devices.

## Pairing

1. Start pairing mode on bridge device. This can be done by setting `homie/{device_id}/rf24/pairing` property to `ON`.     
    ```publish "ON" to homie/{device_id}/rf24/pairing/set```
1. Push `ANNOUNCE` button on sensor node. Sensor node will broadcase its devices.
1. Bridge property `homie/{device_id}/rf24/newdevices` will contain comma delimited list of announced devices which are not paired yet.
1. Pair sensor device with bridge by publishing `pair:{sensor_type}:{sensor_id}` to bridge property `homie/{device_id}/thing/cmd/set`.

