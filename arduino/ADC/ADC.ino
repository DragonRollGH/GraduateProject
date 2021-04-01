#include <NeoPixelBus.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const int Min = 50;
const int Max = 300;

WiFiClient WC;
PubSubClient MQTT(WC);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Pixel(24);

int adj = 12;

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

  adj = payload[0] - 'A';
}


int limit(int x, int min, int max)
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

  int Sensor = analogRead(A0);
  int Light = (Sensor - Min) * 24 / (Max - Min);
  Light += adj - 12;
  Light = limit(Light, 0, 24);
  Pixel.ClearTo(RgbColor(0, 0, 0));
  for (byte i = 0; i < Light; i++)
  {
    Pixel.SetPixelColor(i, RgbColor(10, 10, 10));
  }
  Pixel.Show();
  delay(100);
}
