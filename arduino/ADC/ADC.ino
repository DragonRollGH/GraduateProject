#include <NeoPixelBus.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const unsigned int Min = 50;
const unsigned int Max = 300;

WiFiClient WC;
PubSubClient MQTT(WC);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Pixel(24);

byte adj = 12;

void MQTTConnect(void)
{
    MQTT.connect("Light");
    MQTT.subscribe("Test");
    Serial.println("MQTT connected");
}

void MQTTCallback(const char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(length);
  Serial.print("] ");
  Serial.println();

  if (length == 1)
  {
    adj = payload[0] - 'A';
  }
}


unsigned int limit(unsigned int x, unsigned int min, unsigned int max)
{
  x = x > max ? max : x;
  x = x < min ? min : x;
  return x;
}

void setup()
{
  Serial.begin(115200);

  Pixel.Begin();

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  WiFi.begin("iTongji-manul", "YOUYUAN4411");

  delay(1000);

  MQTT.setServer("192.168.1.110", 1883);
  MQTT.setCallback(MQTTCallback);

  Serial.println("\nESP OK");
}

void loop()
{
  if (!MQTT.connected())
  {
    MQTTConnect();
  }
  MQTT.loop();

  unsigned int Sensor = analogRead(A0);
  Sensor = limit(Sensor, Min, Max);
  char Light = (Sensor - Min) * 24 / (Max - Min);
  Light = Light - 12 + adj;
  Pixel.ClearTo(RgbColor(0, 0, 0));
  for (byte i = 0; i < Light; i++)
  {
    Pixel.SetPixelColor(i, RgbColor(10, 10, 10));
  }
  Pixel.Show();
  delay(100);
}
