/*
 * ESP32 AJAX rangefinder
 * Updates and Gets data from webpage without page refresh
 * https://circuits4you.com
 */
 
// 3rd party libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>

// Embedded web page contents
#include "index.h"    

// Distance reading and angle
int range;     // Latest reading
int angle = 0; // Initial assumed scan angle
int roi;       // region of Intrest setting

// Distance history for rolling average
#define HISTORY_SIZE 10
int history[HISTORY_SIZE];
byte historySpot;

// Settings
bool enabled = false; // main enabled/disabled thing
String mode = "Near"; // range mode

// Wifi
#if __has_include("mywifi.h")
  // I keep my settings in a seperate header file that is not checked into github
  #include "mywifi.h"
#else
  // Leave as is to create an accesspoint, or set ACCESSPOINT 
  //  to false and supply your networks SSID and PASSWORD.
  #define ACCESSPOINT true
  const char* ssid = "VL53L0X";
  const char* password = "accesspoint";
#endif

// Webserver
WebServer server(80);

// Distance Sensor
SFEVL53L1X distanceSensor;
// Uncomment the following lines to use the optional shutdown and interrupt pins.
//#define SHUTDOWN_PIN 2
//#define INTERRUPT_PIN 3
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

// Other
#define LED 2    // On-Board LED pin
#define BLINK 30 // On-Board LED blink time in ms


//===============================================================
// Setup
//===============================================================

void setup(void){
  // Misc. hardware
  pinMode(LED, OUTPUT);

  // Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  
  // Turn the LED on once serial begun
  digitalWrite(LED, HIGH);          

#if ACCESSPOINT
  // Access point 
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);
  Serial.print("Access Point started: ");
  Serial.print(ssid);
  Serial.print(":");
  Serial.println(password);
  Serial.print("AP Address: ");
  Serial.println(WiFi.softAPIP());
#else
  // Connect to existing wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  //Wait for WiFi to connect
  int aline = 0;
  while(WiFi.waitForConnectResult() != WL_CONNECTED)
  {      
    delay(1000);
    aline++;
    if (aline > 80) {
      aline = 0;
      Serial.println(".");
    }
    else
    {
      Serial.print(".");
    }
  } 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
#endif

  // HTTP request responders
  server.on("/", handleRoot);           // Main page
  // Info requests
  server.on("/readRANGE", handleRANGE); // Update of distance
  server.on("/info", handleInfo);       // info
  // Settings
  server.on("/near", handleNearMode);   // mode seting
  server.on("/mid", handleMidMode);     // mode seting
  server.on("/far", handleFarMode);     // mode seting
  // Commands
  server.on("/on", handleOn);           // Sensor Enable
  server.on("/off", handleOff);         // Sensor Disable
  server.on("/left", handleLeft);       // Sensor Enable
  server.on("/right", handleRight);     // Sensor Disable

  server.on("/roiplus", handleRoiPlus);   // widen ROI
  server.on("/roiminus", handleRoiMinus); // narrow ROI
  

  // Start web server
  server.begin();
  Serial.println("HTTP server started");

  //    Start sensor
  Wire.begin();
  if (distanceSensor.begin() == true)
    Serial.println("Sensor online!");

  // read sensor initial values
  roi = distanceSensor.getROIX();
  
  // Clean history..
  for (int x = 0 ; x < HISTORY_SIZE ; x++)
    history[x] = 0;

  // Setup complete, turn LED off
  digitalWrite(LED, LOW);   // turn the LED off now init is successful

  // sensor default mode
  handleNearMode(); 
}


//=========================================
// Handlers for responses to http requests
//=========================================

void handleRoot() {
  digitalWrite(LED, HIGH);   // blink the LED
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
  Serial.print("Sent the main page to: ");
  String addy = server.client().remoteIP().toString();
  Serial.println(addy);
  delay(BLINK);
  digitalWrite(LED, LOW);
  delay(BLINK);
  digitalWrite(LED, HIGH);   // twice
  delay(BLINK);
  digitalWrite(LED, LOW);
}
 
void handleRANGE() {

  // TODO: JONify and send angle
  
  if (enabled == true)
  {
    long startTime = millis();
    distanceSensor.startRanging(); //Write configuration block of 135 bytes to setup a measurement
    int range = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
    distanceSensor.stopRanging();
    long endTime = millis();
    String rangeValue = String(range);
  
    history[historySpot] = range;
    if (++historySpot == HISTORY_SIZE) historySpot = 0;
  
    long average = 0;
    for (int x = 0 ; x < HISTORY_SIZE ; x++)
      average += history[x];
      
    average /= HISTORY_SIZE;
    String rangeAverage = String(average);
  
    server.send(200, "text/plane", rangeValue);
    //server.send(200, "text/plane", rangeAverage);
  }
  else
  {
    server.send(200, "text/plane", "disabled");
  }
}

void handleOn()
{
  enabled = true;
  distanceSensor.startRanging();
  server.send(200, "text/plane", "sensor enabled");
  
  // blink LED and send to serial
  digitalWrite(LED, HIGH);   
  Serial.println("Turning On");
  delay(BLINK);
  digitalWrite(LED, LOW);
}

void handleOff()
{
  enabled = false;
  distanceSensor.stopRanging();
  server.send(200, "text/plane", "sensor disabled");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Turning Off");
  delay(BLINK);
  digitalWrite(LED, LOW);

}

void handleNearMode()
{
  mode = "Near"; 
  distanceSensor.setDistanceModeShort();
  distanceSensor.setTimingBudgetInMs(20);
  distanceSensor.setIntermeasurementPeriod(20);

  server.send(200, "text/plane", "short mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Low");
  delay(BLINK);
  digitalWrite(LED, LOW);
}
void handleMidMode()
{
  mode = "Mid"; 
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(33);
  distanceSensor.setIntermeasurementPeriod(33);
 
  server.send(200, "text/plane", "mid mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Mid");
  delay(BLINK);
  digitalWrite(LED, LOW);
}
void handleFarMode()
{
  mode = "Far"; 
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(50);
  distanceSensor.setIntermeasurementPeriod(50);
  server.send(200, "text/plane", "far mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Far");
  delay(BLINK);
  digitalWrite(LED, LOW);
}

void handleLeft()
{
  int newangle = angle - 10; 
  if (newangle < -180) angle = -180; else angle = newangle;
  server.send(200, "text/plane", "left");
}

void handleRight()
{
  int newangle = angle + 10; 
  if (newangle > 180) angle = 180; else angle = newangle;
  server.send(200, "text/plane", "right");
}

void handleRoiPlus()
{
  int newroi = roi + 1;
  if (newroi > 16) roi = 16; else roi = newroi;
  server.send(200, "text/plane", "roiplus");
  distanceSensor.setROI(roi, roi);
}

void handleRoiMinus()
{
  int newroi = roi - 1;
  if (newroi < 4) roi = 4; else roi = newroi;
  server.send(200, "text/plane", "roiminus");
  distanceSensor.setROI(roi, roi);
}


void handleInfo()
{
  // Todo: JSONify this

  String infoblock = "";
  infoblock += String (distanceSensor.getRangeStatus());
  infoblock += " : ";
  infoblock += String (distanceSensor.getTimingBudgetInMs());
  infoblock += " : ";
  infoblock += String (distanceSensor.getIntermeasurementPeriod());
  infoblock += " : ";
  infoblock += String (distanceSensor.getDistanceMode());
  infoblock += " : ";
  infoblock += String (distanceSensor.getSignalPerSpad());
  infoblock += " : ";
  infoblock += String (distanceSensor.getSpadNb());
  infoblock += " : ";
  infoblock += String (distanceSensor.getROIX());
  infoblock += " : ";
  infoblock += String (distanceSensor.getROIY());
  infoblock += " : angle ";
  infoblock += String (angle);

  server.send(200, "text/plane", infoblock);
}


//====================================
// Main Loop (invokes client handler)
//====================================

void loop(void){
  server.handleClient();
  delay(1);
}
