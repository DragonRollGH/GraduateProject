#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <NeoPixelBus.h>
#include <Servo.h>
#include <Ticker.h>
#include <vector>
#include <WiFiManager.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

const int MQTTPort = 1883;
const char *MQTTServer = "192.168.1.110";
const String MQTTClientid = "Devices";
const String MQTTPub = "devices";
const String MQTTSub[] = {
    "curtain",
    "fan",
    "humidifier",
    "light",
    "window"
};
MQTTClient MQTT;
WiFiClient WLAN;
WiFiManager WM;

const int CurtainMax = 180;
const int CurtainPin = D1;
Servo Curtain;
Ticker CurtainTicker;

const int FanMax = 255;
const int FanPin = D4;

const int HumidifierMax = 1;
const int HumidifierPin = D3;

const int LightMax = 16;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Light(LightMax);//RX

const int WindowMax = 180;
const int WindowPin = D5;
Servo Window;
Ticker WindowTicker;

struct WiFiEntry {
    String SSID;
    String PASS;
};
std::vector<WiFiEntry> WiFiList;

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
        Serial.println(value);
    }
}

void humidifier(int value)
{
    if (0 <= value && value <= HumidifierMax)
    {
        digitalWrite(HumidifierPin, value);
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

void MQTTConnect()
{
    for (byte i = 0; i < 120; ++i)
    {
        WiFiConnect();
        if (MQTT.connect((MQTTClientid + millis()).c_str()))
        {
            break;
        }
        delay(500);
    }
    for (byte i = 0; i < 5; ++i)
    {
        MQTT.subscribe(MQTTSub[i]);
        delay(10);
    }
}

void MQTTInitialize()
{
    MQTT.begin(MQTTServer, MQTTPort, WLAN);
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

void MQTTMsg(String &topic, String &payload)
{
    if (topic == "curtain")
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

void WiFiAdd(String SSID, String PASS)
{
    WiFiList.push_back(WiFiEntry{SSID, PASS});
}

int WiFiConnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        WiFi.scanDelete();
        for (byte k = 0; k < 3; ++k)
        {
            byte scanResult = WiFi.scanNetworks();
            byte bestWiFi = 255;
            if (scanResult > 0)
            {
                int bestRSSI = INT_MIN;
                for (byte i = 0; i < scanResult; ++i)
                {
                    for (byte j = 0; j < WiFiList.size(); ++j)
                    {
                        if (WiFi.SSID(i) == WiFiList[j].SSID)
                        {
                            if (WiFi.RSSI(i) > bestRSSI)
                            {
                                bestWiFi = j;
                            }
                            break;
                        }
                    }
                }
            }
            WiFi.scanDelete();

            if (bestWiFi != 255)
            {
                WiFi.begin(WiFiList[bestWiFi].SSID, WiFiList[bestWiFi].PASS);
                for (byte i = 0; i < 60; ++i)
                {
                    if (WiFi.status() == WL_CONNECTED)
                    {
                        return 1;
                    }
                    delay(500);
                }
            }
        }

        WiFiPortal();
    }
}

void WiFiInitialize()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
    WiFiAdd("iTongji-manul", "YOUYUAN4411");
    WiFiAdd("DragonRoll", "1234567890");
}

int WiFiPortal()
{
    WM.setConfigPortalTimeout(10);
    WM.startConfigPortal(MQTTClientid.c_str());
    if (WiFi.status() == WL_CONNECTED)
    {
        return 1;
    }
    else
    {
        ESP.deepSleepInstant(INT32_MAX);
    }
}

void setup()
{
    WiFiInitialize();
    MQTTInitialize();

    Serial.begin(115200);
    pinMode(CurtainPin, OUTPUT);
    pinMode(FanPin, OUTPUT);
    pinMode(HumidifierPin, OUTPUT);
    pinMode(WindowPin, OUTPUT);
    analogWriteFreq(100);
    analogWriteRange(FanMax);
    Light.Begin();
}

void loop()
{
    MQTTLoop();

    delay(100);
}