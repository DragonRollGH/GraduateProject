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
#define D7 13
#define D8 15

const int MQTTPort = 1883;
const char *MQTTServer = "192.168.1.110";
const char *MQTTClientid = "Devices";
const char *MQTTPub = "devices";
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

const int CurtainMax = 180; //35-130
const int CurtainPin = D1;
Servo Curtain;
Ticker CurtainTicker;

const int FanMax = 255;
const int FanPin = D2;

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

void cmdUpdate(String &paylaod)
{
    String url = "http://www.dr21.fun/DragonRollGH/Pulser/main/arduino/Pulser/Pulser.ino.generic.bin";
    if (paylaod.length() > 2)
    {
        url = paylaod.substring(2);
    }
    MQTT.publish(MQTTPub, "Starting update from " + url);

    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    ESPhttpUpdate.onStart([] { MQTT.publish(MQTTPub, "[httpUpdate] Started"); });
    ESPhttpUpdate.onError([](int err) { MQTT.publish(MQTTPub, String("[httpUpdate] Error: ") + ESPhttpUpdate.getLastErrorString().c_str()); });
    ESPhttpUpdate.update(url);
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
        if (MQTT.connect((String(MQTTClientid) + millis()).c_str()))
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
        // cmdUpdate(payload);
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
        // WiFiConfigNew();
    }
}

void WiFiAdd(String SSID, String PASS)
{
    WiFiList.push_back(WiFiEntry{SSID, PASS});
}

void WiFiConfigNew()
{
    LittleFS.begin();
    StaticJsonDocument<128> doc;
    doc["len"] = 0;
    File WiFiConfig = LittleFS.open("/WiFi.json", "w");
    serializeJson(doc, WiFiConfig);
    WiFiConfig.close();
    LittleFS.end();
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
        StaticJsonDocument<128> doc;
        doc["len"] = 0;
        File WiFiConfig = LittleFS.open("/WiFi.json", "w");
        serializeJson(doc, WiFiConfig);
        WiFiConfig.close();
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
    Serial.println("ESP OK");
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
