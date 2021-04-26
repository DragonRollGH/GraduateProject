#include <DHT.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <NeoPixelBus.h>
#include <Servo.h>
#include <Ticker.h>
#include <vector>
#include <WiFiManager.h>

const int MQTTPort = 1883;
const char *MQTTServer = "192.168.1.110";
const String MQTTClientid = "Sensors";


MQTTClient MQTT;
WiFiClient WLAN;
WiFiManager WM;

DHT Dht;

struct WiFiEntry {
    String SSID;
    String PASS;
};
std::vector<WiFiEntry> WiFiList;

void DhtLoop()
{
  MQTT.publish("humidity", String(Dht.getHumidity()));
  MQTT.publish("temperature", String(Dht.getTemperature()));
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
}

void MQTTInitialize()
{
    MQTT.begin(MQTTServer, MQTTPort, WLAN);
}

void MQTTLoop()
{
    if (!MQTT.connected())
    {
        MQTTConnect();
    }
    MQTT.loop();
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

    Dht.setup(2);
}

void loop()
{
    MQTTLoop();

    delay(100);
}