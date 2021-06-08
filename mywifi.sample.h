// Wifi Network Definitions:
// Copy or rename this file to 'mywifi.h' and edit to provide your own WiFi details

// Conect to an existing network
const char* ssid = "my-wifi";
const char* password = "my-password";

// Or.. 
// Secure accesspoint config
//#define ACCESSPOINT
//#define ACCESSPOINTIP 192,168,4,1
//#define ACCESSPOINTMASK 255,255,255,0
//const char* ssid = "my-demo";
//const char* password = "my-password";

/* Uncomment to set a custom name for the Device */
//#define DEVICENAME "ESP32/VL53L1X Demo"

/*
 * Over The Air firmware updates can be disabled by uncommenting the folowing line
 */
//#define NO_OTA

/*
 * OTA can be password protected to prevent the device being hijacked
 */
//#define OTA_PASSWORD "SuperVisor"
