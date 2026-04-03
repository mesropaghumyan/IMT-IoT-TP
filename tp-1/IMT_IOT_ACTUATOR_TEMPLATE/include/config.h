#include <Arduino.h>

// Whether the given script is using encryption or not,
// generally recommended as it increases security (communication with the server is not in clear
// text anymore), it does come with an overhead tough as having an encrypted session requires a lot
// of memory, which might not be avaialable on lower end devices.
#define ENCRYPTED true

#define MQTT_USER "admin"
#define MQTT_PASS "admin"

// While we wait for Feather ESP32 V2 to get added to the Espressif BSP,
// we have to select PICO D4 and UNCOMMENT this line!
#define ADAFRUIT_FEATHER_ESP32_V2

// Enables the ThingsBoard class to be fully dynamic instead of requiring template arguments to
// statically allocate memory. If enabled the program might be slightly slower and all the memory
// will be placed onto the heap instead of the stack.
// #define THINGSBOARD_ENABLE_DYNAMIC 1

// If the THINGSBOARD_ENABLE_DYNAMIC 1 setting causes this error log message to appear [TB] Unable
// to de-serialize received json data with error (DeserializationError::NoMemory). Simply add this
// configuration line as well.
// #define THINGSBOARD_ENABLE_PSRAM 0

// Enables sending messages that are bigger than the predefined message size,
// where the message will be sent byte by byte as a fallback instead.
// Requires an additional library, see https://github.com/bblanchon/ArduinoStreamUtils for more
// information. Simply install that library and the feature will be enabled automatically.
#define THINGSBOARD_ENABLE_STREAM_UTILS 0

// Baud rate for the debugging serial connection
// If the Serial output is mangled, ensure to change the monitor speed accordingly to this variable
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// Enable Serial debug output
#define SERIAL_DEBUG true

// Enable MQTT RPC debug
#define MQTT_RPC_DEBUG false

// Enable Thingsboard library debug
#define THINGSBOARD_ENABLE_DEBUG false

constexpr char WIFI_SSID[]	   = "thingsboard";
constexpr char WIFI_PASSWORD[] = "thingsboard";

// Module actionneur Thingsboard token access
// Doit être modifié suivant binome
constexpr char device_id[] = "8081";

// Thingsboard server IP address
constexpr char MQTT_SERVER[] = "10.42.0.217";

// MQTT port used to communicate with the server, 1883 is the default unencrypted MQTT port,
// whereas 8883 would be the default encrypted SSL MQTT port
#if ENCRYPTED
constexpr uint16_t THINGSBOARD_PORT = 8883U;
#else
constexpr uint16_t THINGSBOARD_PORT = 1883U;
#endif

// 5. Remplace le contenu de ROOT_CERT par celui de ton ca.crt
#if ENCRYPTED
constexpr char ROOT_CERT[] = R"(-----BEGIN CERTIFICATE-----
MIIDETCCAfmgAwIBAgIUeKDze5aS1L80X+QijvLCaBxzP4EwDQYJKoZIhvcNAQEL
BQAwGDEWMBQGA1UEAwwNTXlNb3NxdWl0dG9DQTAeFw0yNjA0MDMxMzEzMDBaFw0z
NjAzMzExMzEzMDBaMBgxFjAUBgNVBAMMDU15TW9zcXVpdHRvQ0EwggEiMA0GCSqG
SIb3DQEBAQUAA4IBDwAwggEKAoIBAQC0NA2WoXGCKqaCbjjL6VWBidEjmJHJZZZE
EFbadTNJPfLmPolq84eV0AgZFO+adASHUMUzfJDKtY0NoC/JjJs4L1XdDmNsVb8b
fOuCmXJKy1CW08Nx/UE3k6VlmtrzvNIlMrSIhCkHmu837AlWySiC1J6T84tkdyel
0+OyJ204yhAnfYNxGfFaptffI1bWUxBFkwom+q8fb3pDsbiXS4V+9aWMtWiHMXZ3
bx568ZXFhZcTOSEolicpvO+V3pfovmRC8PX3Vjw3rugVs15Av1BDehnSxioxnEn0
PuldnpTjVtNRhU4oPadL3nYjp25afxh136uBvRrW31qwDvrRDOwtAgMBAAGjUzBR
MB0GA1UdDgQWBBS36JiLXCdIVsUkwUjDrAPUB0bRwjAfBgNVHSMEGDAWgBS36JiL
XCdIVsUkwUjDrAPUB0bRwjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA
A4IBAQAaBffO6+EqMKq6zPrmRIIL8br2RAvNs1EuLPxih1UasHoAeqQlRRKzbddr
bwNNElpVdcvzPsHGz1b0HOHHiDQSBtO2EGOhPvpMZp2JERzi6gQ/wuy+Qv4RAOfw
no+d9qaDQTFzKN0TDEhaM6//fjHCyBx2FNZrVJ4hWl/VbgvOoyi+sw/YJaBtoY1R
yNd+e/dqJcvnDybh5d2puzPbVwGRGo6CN9c7TX9H4SacNi+EnPnh5hCW4G4lGAxU
xVTzbT6DXPKjuHx1VVEflFCxh3uCvpDfs886DqIoQ99vhTAE2gP+bv/XChlmq3Dq
NKPCHhBBqygugl9O41S2eqoCZEth
-----END CERTIFICATE-----
)";
#endif

// VMC, LIGHT, HEATER & AC control pins
#define VMC_PIN 12
#define LIGHT_PIN 13
#define HEATER_PIN 27
#define AC_PIN 33