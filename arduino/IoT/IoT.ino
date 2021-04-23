#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <MQTT.h>
#include <NeoPixelBus.h>
#include <Servo.h>
#include <Ticker.h>
#include <WiFiManager.h>

const String MQTTPub = "IoT_ESP"
const String MQTTSub[] = {
    "curtain",
    "fan",
    "humidifier",
    "light",
    "window"
};
ESP8266WiFiMulti STA;
MQTTClient MQTT;
WiFiClient WLAN;
WiFiManager WM;

const int CurtainMax = 180;
const int CurtainPin = 4;
Servo Curtain;
Ticker CurtainTicker;

const int FanMax = 255;
const int FanPin = 8;

const int HumidifierMax = 1;
const int HumidifierPin = 10;

const int LightMax = 16;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Light(LightMax);

const int WindowMax = 180;
const int WindowPin = 5;
Servo Window;
Ticker WindowTicker;


void curtain(int value)
{
    if (0 <= value && value <= CurtainMax)
    {
        Curtain.attach(CurtainPin);
        Curtain.write(value);
        CurtainTicker.once_ms(500, []() {
            Curtain.detach();
        });
    }
}

void fan(int value)
{
    if (0 <= value && value <= FanMax)
    {
        analogWrite(FanPin, value);
    }
}

void humidifier(int value)
{
    if (0 <= value && value <= HumidifierMax)
    {
        digitalWrite(HumidifierPin, value);
    }
}

void curtain(int value)
{
    if (0 <= value && value <= CurtainMax)
    {
        Curtain.attach(CurtainPin);
        Curtain.write(value);
        CurtainTicker.once_ms(500, []() {
            Curtain.detach();
        });
    }
}

void light(int value)
{
    if (0 <= value && value <= LightMax)
    {
        Light.ClearTo(RgbColor(0, 0, 0));
        for (byte i = 0; i < value; i++)
        {
            Light.SetPixelColor(i, RgbColor(10, 10, 10));
        }
        Light.Show();
    }
}

void window(int value)
{
    if (0 <= value && value <= WindowMax)
    {
        Window.attach(WindowPin);
        Window.write(value);
        WindowTicker.once_ms(500, []() {
            Window.detach();
        });
    }
}

void MQTTMsg(String &topic, String &payload)
{
    else if (topic == "curtain")
    {
        curtain(payload.toInt());
    }
    else if (topic == "fan")
    {
        fan(payload.toInt());
    }
    else if (topic == "humidifier")
    {
        humidifier(payload.toInt());
    }
    else if (topic == "light")
    {
        light(payload.toInt());
    }
    else if (topic == "window")
    {
        window(payload.toInt());
    }

}

void MQTTConnect()
{
    for (byte i = 0; i < 120; ++i)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            WiFiConnect();
        }
        if (MQTT.connect("IoT"))
        {
            break;
        }
        delay(500);
    }
    MQTT.subscribe(,MQTTSub1);
    delay(10);
    MQTT.subscribe(MQTTSub2);
}

void MQTTInitialize()
{
    MQTT.begin("192.168.1.110", WLAN);
    MQTT.onMessage(MQTTMsg);
}

void MQTTLoop()
{
    if (!MQTT.connected())
    {
        MQTTConnect();
    }
    MQTT.loop();
}

int WiFiConnect()
{
    for (byte i = 0; i < 30; ++i)
    {
        if (STA.run() == WL_CONNECTED)
        {
            return 1;
        }
        delay(500);
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        WM.setConfigPortalTimeout(180);
        WM.startConfigPortal(AP_SSID);
        if (WiFi.status() == WL_CONNECTED)
        {
            return 1;
        }
        else
        {
            ESP.deepSleepMax();
        }
    }
}

void WiFiInitialize()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
    STA.addAP("iTongji-manul", "YOUYUAN4411");
    STA.addAP("DragonRoll", "1234567890");
}

void setup()
{
    WiFiInitialize();
    MQTTInitialize();

    analogWriteFreq(100);
    Light.Begin();
}

void loop()
{
    MQTTLoop();

    delay(100);
}