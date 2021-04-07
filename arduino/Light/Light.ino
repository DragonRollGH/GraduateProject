#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <NeoPixelBus.h>
#include <Servo.h>

const int PixelLen = 24;

int adc;
int lastAdc = 0;
int pinServo = 4;

WiFiClient WLAN;
MQTTClient MQTT;
Servo MG90;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Pixel(PixelLen);

void mqttConnect(void)
{
  while (!MQTT.connect("Light"))
  {
    delay(500);
  }
  MQTT.subscribe("light");
  MQTT.subscribe("curtain");
  Serial.println("MQTT connected");
}

void mqttMsg(String &topic, String &payload)
{
  Serial.println("Message arrived [" + topic + "] " + payload);

  if (topic == "light")
  {
    int value = payload.toInt();
    if (0 <= value && value <= 24)
    {
      setPixels(value);
    }
  }
  else if (topic == "curtain")
  {
    int value = payload.toInt();
    if (0 <= value && value <= 180)
    {
      setCurtain(value);
    }
  }
}


void setPixels(int value)
{
  Pixel.ClearTo(RgbColor(0, 0, 0));
  for (byte i = 0; i < value; i++)
  {
    Pixel.SetPixelColor(i, RgbColor(10, 10, 10));
  }
  Pixel.Show();
}

void setCurtain(int value)
{
  MG90.attach(pinServo);
  MG90.write(value);
  delay(300);
  MG90.detach();
}


void sensor(void)
{
  int adcs = 0;
  for (byte i = 0; i < 10; i++)
  {
    adcs += analogRead(A0);
  }
  adc = adcs / 10;
  if (abs(adc - lastAdc) > 20)
  {
    Serial.println(adc);
    MQTT.publish("lightsensor", String(adc));
    lastAdc = adc;
  }
}

void setup()
{
  Serial.begin(115200);

  Pixel.Begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin("iTongji-manul", "YOUYUAN4411");

  delay(1000);

  MQTT.begin("192.168.1.110", WLAN);
  MQTT.onMessage(mqttMsg);

  Serial.println("\nESP OK");
}

void loop()
{
  if (!MQTT.connected())
  {
    mqttConnect();
  }
  MQTT.loop();

  sensor();
  delay(100);
}