/*
 * Generated using BlocklyDuino:
 *
 * https://github.com/MediaTek-Labs/BlocklyDuino-for-LinkIt
 *
 * Date: Thu, 31 Dec 2020 05:06:19 GMT
 */

/*

 * 部份程式碼由吉哥積木產生
 * https://sites.google.com/jes.mlc.edu.tw/ljj/linkit7697

*/

#include <Adafruit_NeoPixel.h>

#include "TinyGsmClientSIM7020.h"
#include <PubSubClient.h>
#include <DHT.h>

int red;

int green;

int blue;

int i;

unsigned long timer;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8,3,NEO_GRB + NEO_KHZ800);

typedef TinyGsmSim7020 TinyGsm;
typedef TinyGsmSim7020::GsmClientSim7020 TinyGsmClient;

TinyGsm       modem(Serial1, 5);

void nbConnect(void)
{
    Serial.println(F("Initializing modem..."));
    while (!modem.init() || !modem.nbiotConnect("twm.nbiot", 28)) {
        Serial.print(F("."));
    }

    Serial.print(F("Waiting for network..."));
    while (!modem.waitForNetwork()) {
        Serial.print(F("."));
    }
    Serial.println(F(" success"));
}

const char* broker = "io.adafruit.com";
const int port = 1883;
const char* mqtt_id = "20201230_Linkit7697_am7020_MQTTID";
const char* mqtt_username = "<YOUR USERNAME>";
const char* mqtt_password = "<YOUR AIO KEY>";

String topic_buff;

String msg_buff;

TinyGsmClient tcpClient(modem);
PubSubClient  mqttClient(broker, port, tcpClient);

void mqttConnect(void)
{
    Serial.print(F("Connecting to "));
    Serial.print(broker);
    Serial.print(F("..."));

    while (!mqttClient.connect(mqtt_id, mqtt_username, mqtt_password)) {
        Serial.print(F(" fail"));
    }
    Serial.println(F(" success"));
}

DHT dht11_p2(2, DHT11);

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    topic_buff = String(topic);
    msg_buff = "";

    for (int ii = 0;ii < (int)len;ii++) {
        msg_buff += (char)(*(payload + ii));
    }
    Serial.print(F("Message arrived ["));
    Serial.print(topic_buff);
    Serial.print(F("]: "));
    Serial.println(msg_buff);

  if (topic_buff == "<YOUR USERNAME>/feeds/linkit7697.red") {
    red = String(msg_buff).toInt();

  } else if (topic_buff == "<YOUR USERNAME>/feeds/linkit7697.green") {
    green = String(msg_buff).toInt();
  } else if (topic_buff == "<YOUR USERNAME>/feeds/linkit7697.blue") {
    blue = String(msg_buff).toInt();
  } else {
    Serial.println("unknown topic");

  }
  for (i = 0; i <= 8; i++) {
    pixels.setPixelColor(i, pixels.Color(red,green,blue));
  }
  pixels.show();

}

void setup()
{
  Serial.begin(115200);

  pixels.begin();

  pixels.setBrightness(255);

  Serial1.begin(115200);

  randomSeed(analogRead(16));

  topic_buff.reserve(100);

  msg_buff.reserve(100);

  mqttClient.setCallback(mqttCallback);

  mqttClient.setKeepAlive(290);

  red = 0;
  green = 0;
  blue = 0;
  timer = 0;
  nbConnect();
  mqttConnect();
  mqttClient.subscribe(String("<YOUR USERNAME>/feeds/linkit7697.red").c_str());
  mqttClient.subscribe(String("<YOUR USERNAME>/feeds/linkit7697.green").c_str());
  mqttClient.subscribe(String("<YOUR USERNAME>/feeds/linkit7697.blue").c_str());
  dht11_p2.begin();
}


void loop()
{
  if (!modem.isNetworkConnected()) {
    nbConnect();

  }
  if (!mqttClient.connected()) {
    mqttConnect();

  }
  if (millis() > timer) {
    timer = millis() + 60000;
    mqttClient.publish(String("<YOUR USERNAME>/feeds/linkit7697.temperature").c_str(), String(dht11_p2.readTemperature()).c_str());
    mqttClient.publish(String("<YOUR USERNAME>/feeds/linkit7697.humidity").c_str(), String(dht11_p2.readHumidity()).c_str());

  }
  mqttClient.loop();
}