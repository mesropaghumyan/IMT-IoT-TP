#include "config.h"
#include "version.h"

#include <Arduino.h>
#include <WiFi.h>
#if ENCRYPTED
#include <WiFiClientSecure.h>
#else
#include <WiFiClient.h>
#endif
#include <PubSubClient.h>
#include <ArduinoJson.h>

// -----------------------------------------------------------------------------
// Notes:
// - device_id from config.h is used as the device_id. Nothing is hardcoded.
// - The actuator publishes its real state on:
//     imt_test/actuator/<device_id>/state
// - The actuator listens for commands on:
//     imt_test/actuator/<device_id>/set
// -----------------------------------------------------------------------------

namespace
{

constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000U;
constexpr uint32_t MQTT_CONNECT_TIMEOUT_MS = 10000U;
constexpr uint32_t MQTT_RECONNECT_DELAY_MS = 1000U;

bool g_vmcState	   = false;
bool g_lightState  = false;
bool g_heaterState = false;
bool g_acState	   = false;

String stateTopic;
String setTopic;

#if ENCRYPTED
WiFiClientSecure netClient;
#else
WiFiClient netClient;
#endif
PubSubClient mqttClient(netClient);

void initOutputs()
{
	pinMode(VMC_PIN, OUTPUT);
	pinMode(LIGHT_PIN, OUTPUT);
	pinMode(HEATER_PIN, OUTPUT);
	pinMode(AC_PIN, OUTPUT);

	digitalWrite(VMC_PIN, LOW);
	digitalWrite(LIGHT_PIN, LOW);
	digitalWrite(HEATER_PIN, LOW);
	digitalWrite(AC_PIN, LOW);

	g_vmcState	  = false;
	g_lightState  = false;
	g_heaterState = false;
	g_acState	  = false;
}

bool setVmc(const bool status)
{
	digitalWrite(VMC_PIN, status ? HIGH : LOW);
	g_vmcState = status;
	return g_vmcState;
}

bool setLight(const bool status)
{
	digitalWrite(LIGHT_PIN, status ? HIGH : LOW);
	g_lightState = status;
	return g_lightState;
}

bool setHeater(const bool status)
{
	digitalWrite(HEATER_PIN, status ? HIGH : LOW);
	g_heaterState = status;
	return g_heaterState;
}

bool setAc(const bool status)
{
	digitalWrite(AC_PIN, status ? HIGH : LOW);
	g_acState = status;
	return g_acState;
}

void buildTopics()
{
	stateTopic = String("imt_test/actuator/") + device_id + "/state";
	setTopic   = String("imt_test/actuator/") + device_id + "/set";
}

bool publishState()
{
	StaticJsonDocument<256> doc;
	doc["device_id"] = device_id;
	doc["vmc"]		 = g_vmcState;
	doc["heating"]	 = g_heaterState;
	doc["lighting"]	 = g_lightState;
	doc["cooling"]	 = g_acState;
	doc["version"]	 = VERSION;

	char		 payload[256];
	const size_t len = serializeJson(doc, payload, sizeof(payload));

#if SERIAL_DEBUG
	Serial.print("Publish state topic: ");
	Serial.println(stateTopic);
	Serial.print("Payload: ");
	Serial.println(payload);
#endif

	return mqttClient.publish(
		stateTopic.c_str(), reinterpret_cast<const uint8_t*>(payload), len, false);
}

void applyCommandJson(const JsonDocument& doc)
{
	if (doc.containsKey("vmc"))
	{
		setVmc(doc["vmc"].as<bool>());
	}
	if (doc.containsKey("heating"))
	{
		setHeater(doc["heating"].as<bool>());
	}
	if (doc.containsKey("lighting"))
	{
		setLight(doc["lighting"].as<bool>());
	}
	if (doc.containsKey("cooling"))
	{
		setAc(doc["cooling"].as<bool>());
	}
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
#if SERIAL_DEBUG
	Serial.print("MQTT rx topic: ");
	Serial.println(topic);
#endif

	if (String(topic) != setTopic)
	{
#if SERIAL_DEBUG
		Serial.println("Ignoring message on unexpected topic");
#endif
		return;
	}

	StaticJsonDocument<256> doc;
	const auto				err = deserializeJson(doc, payload, length);
	if (err)
	{
#if SERIAL_DEBUG
		Serial.print("JSON parse error: ");
		Serial.println(err.c_str());
#endif
		return;
	}

	applyCommandJson(doc);
	publishState();
}

bool connectWiFi()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		return true;
	}

#if SERIAL_DEBUG
	Serial.print("Connecting to WiFi SSID: ");
	Serial.println(WIFI_SSID);
#endif

	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	const uint32_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(250);
#if SERIAL_DEBUG
		Serial.print('.');
#endif
		if ((millis() - start) > WIFI_CONNECT_TIMEOUT_MS)
		{
#if SERIAL_DEBUG
			Serial.println();
			Serial.println("WiFi connection timeout");
#endif
			return false;
		}
	}

#if SERIAL_DEBUG
	Serial.println();
	Serial.print("WiFi connected, IP: ");
	Serial.println(WiFi.localIP());
#endif
	return true;
}

bool connectMqtt()
{
	if (mqttClient.connected())
	{
		return true;
	}

#if ENCRYPTED
	netClient.setCACert(ROOT_CERT);
#endif

	mqttClient.setServer(MQTT_SERVER, THINGSBOARD_PORT);
	mqttClient.setCallback(mqttCallback);

	const String   clientId = String("actuator-") + device_id;
	const uint32_t start	= millis();

#if SERIAL_DEBUG
	Serial.print("Connecting to MQTT broker ");
	Serial.print(MQTT_SERVER);
	Serial.print(":");
	Serial.println(THINGSBOARD_PORT);
#endif

	while (!mqttClient.connected())
	{
		if (mqttClient.connect(clientId.c_str()))
		{
#if SERIAL_DEBUG
			Serial.println("MQTT connected");
#endif
			if (!mqttClient.subscribe(setTopic.c_str()))
			{
#if SERIAL_DEBUG
				Serial.println("MQTT subscribe failed");
#endif
				return false;
			}
#if SERIAL_DEBUG
			Serial.print("Subscribed to: ");
			Serial.println(setTopic);
#endif
			publishState();
			return true;
		}

		if ((millis() - start) > MQTT_CONNECT_TIMEOUT_MS)
		{
#if SERIAL_DEBUG
			Serial.print("MQTT connection timeout, rc=");
			Serial.println(mqttClient.state());
#endif
			return false;
		}

		delay(500);
#if SERIAL_DEBUG
		Serial.print('.');
#endif
	}

	return true;
}

}  // namespace

void setup()
{
#if SERIAL_DEBUG
	Serial.begin(SERIAL_DEBUG_BAUD);
	delay(300);
	Serial.println();
	Serial.println("Actuator firmware startup");
#endif

	initOutputs();
	buildTopics();

	connectWiFi();
	connectMqtt();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }

    if (!mqttClient.connected()) {
        connectMqtt();
    }

    mqttClient.loop();

    delay(10);
}