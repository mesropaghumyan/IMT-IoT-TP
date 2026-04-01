// AHT20 SENSOR ENABLE / DISABLE
#define AHT20_ENABLE true

// SGP40 SENSOR ENABLE / DISABLE
#define SGP40_ENABLE true

// BH1750 SENSOR ENABLE / DISABLE
#define BH1750_ENABLE true

// BATTERY TESTING ENABLE / DISABLE
#define BAT_TEST_ENABLE true

// Thingsboard library debug
#define THINGSBOARD_ENABLE_DEBUG true

// Serial debug output
#define SERIAL_DEBUG true
#if SERIAL_DEBUG
#define SERIAL_PRINT_SENSOR_VALUES false
#endif

constexpr char WIFI_SSID[]	   = "thingsboard";
constexpr char WIFI_PASSWORD[] = "thingsboard";

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access device_id
constexpr char device_id[] = "8081";
// constexpr char mqtt_attribute_topic[] = "v1/devices/me/attributes";

// MQTT server
constexpr char MQTT_SERVER[] = "10.42.0.2";

// Whether the given script is using encryption or not,
// generally recommended as it increases security (communication with the server is not in clear
// text anymore), it does come with an overhead tough as having an encrypted session requires a lot
// of memory, which might not be avaialable on lower end devices.
#define ENCRYPTED false

// While we wait for Feather ESP32 V2 to get added to the Espressif BSP,
// we have to select PICO D4 and UNCOMMENT this line!
#define ADAFRUIT_FEATHER_ESP32_V2

// Sending data can either be done over MQTT and the PubSubClient
// or HTTPS and the HTTPClient, when using the ESP32 or ESP8266
#define USING_HTTPS false

// Enables the ThingsBoard class to be fully dynamic instead of requiring template arguments to
// statically allocate memory. If enabled the program might be slightly slower and all the memory
// will be placed onto the heap instead of the stack.
#define THINGSBOARD_ENABLE_DYNAMIC 1

// If the THINGSBOARD_ENABLE_DYNAMIC 1 setting causes this error log message to appear [TB] Unable
// to de-serialize received json data with error (DeserializationError::NoMemory). Simply add this
// configuration line as well.
// #define THINGSBOARD_ENABLE_PSRAM 0

// Enables sending messages that are bigger than the predefined message size,
// where the message will be sent byte by byte as a fallback instead.
// Requires an additional library, see https://github.com/bblanchon/ArduinoStreamUtils for more
// information. Simply install that library and the feature will be enabled automatically.
#define THINGSBOARD_ENABLE_STREAM_UTILS 0