// #include <ESP8266WiFi.h>
// #include <MQTT.h>
// #include <NeoPixelBus.h>
// #include <Servo.h>

// const int PixelLen = 24;

// int adc;
// int lastAdc = 0;

// WiFiClient WLAN;
// MQTTClient MQTT;
// Servo MG90;

// NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Pixel(PixelLen);

// void mqttConnect(void)
// {
//   while (!MQTT.connect("Light"))
//   {
//     delay(500);
//   }
//   MQTT.subscribe("light");
//   Serial.println("MQTT connected");
// }

// void mqttMsg(String &topic, String &payload)
// {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   Serial.print(payload);
//   Serial.println();

//   int value = payload.toInt();
//   if (0 <= value && value <= 24)
//   {
//     setPixels(value);
//   }
// }

// void setPixels(int value)
// {
//   Pixel.ClearTo(RgbColor(0, 0, 0));
//   for (byte i = 0; i < value; i++)
//   {
//     Pixel.SetPixelColor(i, RgbColor(10, 10, 10));
//   }
//   Pixel.Show();
// }

// void sensor(void)
// {
//   int adcs = 0;
//   for (byte i = 0; i < 10; i++)
//   {
//     adcs += analogRead(A0);
//   }
//   adc = adcs / 10;
//   if (abs(adc - lastAdc) > 20)
//   {
//     Serial.println(adc);
//     MQTT.publish("lightsensor", String(adc));
//     lastAdc = adc;
//   }
// }

// void setup()
// {
//   Serial.begin(115200);

//   Pixel.Begin();

//   MG90.attach(4);
//   MG90.write(90);

//   WiFi.mode(WIFI_STA);
//   // WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
//   WiFi.begin("iTongji-manul", "YOUYUAN4411");

//   delay(1000);

//   MQTT.begin("192.168.1.110", WLAN);
//   MQTT.onMessage(mqttMsg);


//   Serial.println("\nESP OK");
// }

// void loop()
// {
//   if (!MQTT.connected())
//   {
//     mqttConnect();
//   }
//   MQTT.loop();

//   sensor();
//   delay(100);
// }


#include <Servo.h>


Servo MG90;

void setup()
{
  Serial.begin(115200);

  MG90.attach(4);

  Serial.println("\nESP OK");
}

void loop()
{
  MG90.write(0);
  delay(1000);
  MG90.write(180);
  delay(1000);
}
