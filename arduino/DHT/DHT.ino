#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <DHT.h>

WiFiClient WLAN;
MQTTClient MQTT;
DHT Dht;

void mqttConnect(void)
{
  while (!MQTT.connect("Dht"))
  {
    delay(500);
  }
}

void setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  WiFi.begin("iTongji-manul", "YOUYUAN4411");

  delay(1000);

  MQTT.begin("192.168.1.110", WLAN);

  Dht.setup(2);
}

void loop()
{
  if (!MQTT.connected())
  {
    mqttConnect();
  }
  MQTT.loop();

  MQTT.publish("dht", String(Dht.getHumidity()) + ';' + String(Dht.getTemperature()));
  delay(10000);
}