#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <LittleFS.h>
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

const int MQTTPort = 1883;
const char *MQTTServer = "192.168.1.110";
const char *MQTTClientid = "Devices";
const char *MQTTPub = "devices";
const String MQTTSub[] = {
    "devices",
    "curtain",
    "fan",
    "humidifier",
    "light",
    "window"
};
const int MQTTSubLen = 6;
MQTTClient MQTT;
WiFiClient WLAN;
WiFiManager WM;

const int CurtainMax = 100;
const int CurtainAngleMin = 35;
const int CurtainAngleMax = 130;
const int CurtainPin = D1;
Servo Curtain;
Ticker CurtainTicker;

const int FanMax = 1;
const int FanPin = D2;

const int HumidifierMax = 1;
const int HumidifierPin = D3;

const int LightMax = 16;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Light(LightMax); //RX

const int WindowMax = 100;
const int WindowAngleMin = 35;
const int WindowAngleMax = 130;
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
    value = constrain(value, 0, CurtainMax);
    value = map(value, 0, CurtainMax, CurtainAngleMin, CurtainAngleMax);
    Curtain.attach(CurtainPin);
    Curtain.write(value);
    CurtainTicker.once_ms(1000, []() {
        Curtain.detach();
    });
}

void fan(int value)
{
    value = constrain(value, 0, FanMax);
    digitalWrite(FanPin, value);
}

void humidifier(int value)
{
    value = constrain(value, 0, HumidifierMax);
    digitalWrite(HumidifierPin, value);
}

void light(int value)
{
    value = constrain(value, 0, LightMax);
    Light.ClearTo(RgbColor(0, 0, 0));
    for (byte i = 0; i < value; i++)
    {
        Light.SetPixelColor(i, RgbColor(10, 10, 10));
    }
    Light.Show();
}

void window(int value)
{
    value = constrain(value, 0, WindowMax);
    value = map(value, 0, WindowMax, WindowAngleMin, WindowAngleMax);
    Window.attach(WindowPin);
    Window.write(value);
    WindowTicker.once_ms(1000, []() {
        Window.detach();
    });
}

void MQTTConnect()
{
    for (byte i = 0; i < 120; ++i)
    {
        WiFiConnect();
        if (MQTT.connect((String(MQTTClientid) + millis()).c_str()))
        {
            break;
        }
        delay(500);
    }
    for (byte i = 0; i < MQTTSubLen; ++i)
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
    if (topic == MQTTSub[0] && payload[0] == 'N')
    {
        LittleFS.begin();
        WiFiConfigNew();
        LittleFS.end();
    }
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

void WiFiAdd(String SSID, String PASS)
{
    WiFiList.push_back(WiFiEntry{SSID, PASS});
}

void WiFiConfigNew()
{
    StaticJsonDocument<128> doc;
    doc["len"] = 0;
    File WiFiConfig = LittleFS.open("/WiFi.json", "w");
    serializeJson(doc, WiFiConfig);
    WiFiConfig.close();
}

void WiFiConfigRead()
{
    LittleFS.begin();
    if (LittleFS.exists("/WiFi.json"))
    {
        StaticJsonDocument<512> doc;
        File WiFiConfig = LittleFS.open("/WiFi.json", "r");
        deserializeJson(doc, WiFiConfig);
        for (byte i = 0; i < doc["len"]; ++i)
        {
            WiFiAdd(doc["ssid"][i], doc["pass"][i]);
        }
        WiFiConfig.close();
    }
    else
    {
        WiFiConfigNew();
    }
    LittleFS.end();
}

void WiFiConfigWrite(String SSID, String PASS)
{
    LittleFS.begin();
    File WiFiConfig = LittleFS.open("/WiFi.json", "r");
    StaticJsonDocument<512> doc;
    deserializeJson(doc, WiFiConfig);
    WiFiConfig.close();
    byte len = doc["len"];
    bool exist = 0;
    for (byte i = 0; i < len; ++i)
    {
        if (doc["ssid"][i] == SSID)
        {
            exist = 1;
            if (doc["pass"][i] != PASS)
            {
                doc["pass"][i] = PASS;
            }
        }
    }
    if (!exist)
    {
        ++len;
        doc["len"] = len;
        doc["ssid"].add(SSID);
        doc["pass"].add(PASS);
    }
    WiFiConfig = LittleFS.open("/WiFi.json", "w");
    serializeJson(doc, WiFiConfig);
    WiFiConfig.close();
    LittleFS.end();
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
    WiFiConfigRead();
}

int WiFiPortal()
{
    WM.setConfigPortalTimeout(180);
    WM.startConfigPortal(MQTTClientid);
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiConfigWrite(WM.getWiFiSSID(), WM.getWiFiPass());
        WiFiAdd(WM.getWiFiSSID(), WM.getWiFiPass());
        return 1;
    }
    else
    {
        ESP.deepSleepInstant(INT32_MAX);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("");
    WiFiInitialize();
    MQTTInitialize();

    pinMode(CurtainPin, OUTPUT);
    pinMode(FanPin, OUTPUT);
    pinMode(HumidifierPin, OUTPUT);
    pinMode(WindowPin, OUTPUT);
    Light.Begin();

    Serial.println("ESP OK");
}

void loop()
{
    MQTTLoop();

    delay(100);
}
