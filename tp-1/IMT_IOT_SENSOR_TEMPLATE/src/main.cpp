#include "config.h"
#include "version.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// AHT20 Temperature & Humidity sensor
#include <Adafruit_AHTX0.h>

// SGP40 Air Quality Sensor
#include <Adafruit_SGP40.h>

// BH1750 Luxmeter
#include <hp_BH1750.h>


// Battery pin
#define VBATPIN A13

// then these pins will be defined for us
#if defined(ADAFRUIT_FEATHER_ESP32_V2) || defined(ARDUINO_ADAFRUIT_ITSYBITSY_ESP32)
#define PIN_NEOPIXEL 0
#define NEOPIXEL_I2C_POWER 2
#endif

#define TIME_TO_SLEEP_MS 30000 /* Time between sensor readings (in milliseconds) */
#define SECONDS_80 80000


void InitWiFi();
bool reconnect_wifi();
bool connectMQTT();
bool publishTelemetry(float temperature, float humidity, int32_t voc_index, float lux);

// MQTT broker port used to communicate with the Ubuntu server
#if ENCRYPTED
constexpr uint16_t MQTT_PORT = 8884U;
#else
constexpr uint16_t MQTT_PORT = 1884U;
#endif

// Baud rate for the debugging serial connection
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// MQTT topic prefix
constexpr char MQTT_TOPIC_PREFIX[] = "imt_test/sensor/";

#if ENCRYPTED
constexpr char ROOT_CERT[] = R"(-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)";
#endif

// Initialize underlying client, used to establish a connection
#if ENCRYPTED
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

// Initialize standard MQTT client
PubSubClient mqttClient(espClient);

// AHT20 Temperature & Humidity sensor
Adafruit_AHTX0 aht;

// SGP40 Air Quality Sensor
Adafruit_SGP40 sgp;

// BH1750 Luxmeter
hp_BH1750 BH1750;

float measuredvbat;


void setup()
{
	randomSeed(analogRead(0));

	Serial.begin(SERIAL_DEBUG_BAUD);
	delay(200);


	if (!aht.begin())
	{
		Serial.println("AHT sensor not found !");
		while (true) {}
	}

	Serial.println("AHT10 or AHT20 found");

	if (!sgp.begin())
	{
		Serial.println("SGP40 sensor not found !");
		while (true) {}
	}

	Serial.print("SGP40 found serial #");
	Serial.print(sgp.serialnumber[0], HEX);
	Serial.print(sgp.serialnumber[1], HEX);
	Serial.println(sgp.serialnumber[2], HEX);

	if (!BH1750.begin(BH1750_TO_GROUND))
	{
		Serial.println("BH1750 sensor not found !");
		while (true) {}
	}

	Serial.println("BH1750 sensor found");
	Serial.printf("BH1750 conversion time: %dms\n", BH1750.getMtregTime());

	BH1750.start();

	InitWiFi();

	// Warm up sensors for accuracy
	Serial.println("Waiting for 80 sec. for sensors accuracy ...");
	sensors_event_t humidity, temperature;
	while (SECONDS_80 > millis())
	{
		aht.getEvent(&humidity, &temperature);
		uint16_t sraw = sgp.measureRaw(temperature.temperature, humidity.relative_humidity);
		sgp.measureVocIndex(temperature.temperature, humidity.relative_humidity);

		if (BH1750.hasValue())
		{
			BH1750.getLux();
			BH1750.start();
		}
		delay(100);
	}
}

void loop() 
{
    Serial.println("--- Nouveau cycle de lecture ---");

    reconnect_wifi();

    sensors_event_t humidity_event, temp_event;
    aht.getEvent(&humidity_event, &temp_event);
    float temperature = temp_event.temperature;
    float humidity = humidity_event.relative_humidity;

    int32_t voc_index = sgp.measureVocIndex(temperature, humidity);

    float lux = 0.0;
    if (BH1750.hasValue()) {
        lux = BH1750.getLux();
        BH1750.start();
    }

    if (connectMQTT())
    {
        bool success = publishTelemetry(temperature, humidity, voc_index, lux);
        
        if (success) {
            Serial.println("Données publiées avec succès !");
        } else {
            Serial.println("Échec de la publication.");
        }

        mqttClient.disconnect();
        Serial.println("MQTT déconnecté.");
    }

    Serial.printf("Attente de %d ms...\n", TIME_TO_SLEEP_MS);
    delay(TIME_TO_SLEEP_MS);
}

void InitWiFi()
{
	Serial.println("Connecting to AP ...");

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.printf("\nConnected to AP : %s\n", WIFI_SSID);
}

bool reconnect_wifi()
{
	const wl_status_t status = WiFi.status();
	if (status == WL_CONNECTED)
	{
		return true;
	}

	InitWiFi();
	return true;
}

bool connectMQTT()
{
	mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

	String clientId = "sensor-";
	clientId += device_id;

	Serial.printf("Connecting to MQTT broker %s:%u\n", MQTT_SERVER, MQTT_PORT);

	uint32_t start = millis();

	while (!mqttClient.connected())
	{
		if (mqttClient.connect(clientId.c_str()))
		{
			Serial.println("MQTT connected");
			return true;
		}

		if ((millis() - start) > 10000)
		{
			Serial.println("MQTT connection timeout");
			return false;
		}

		delay(500);
		Serial.print(".");
	}

	return true;
}

bool publishTelemetry(float temperature, float humidity, int32_t voc_index, float lux)
{
	String topic = String(MQTT_TOPIC_PREFIX) + device_id + "/telemetry";

	StaticJsonDocument<256> doc;
	doc["device_id"]   = device_id;
	doc["temperature"] = temperature;
	doc["humidity"]	   = humidity;
	doc["voc_index"]   = voc_index;
	doc["lux"]		   = lux;


	doc["version"] = VERSION;

	char		 payload[256];
	const size_t len = serializeJson(doc, payload, sizeof(payload));

	Serial.print("Publishing topic: ");
	Serial.println(topic);
	Serial.print("Payload: ");
	Serial.println(payload);

	return mqttClient.publish(topic.c_str(), payload, len);
}
