/*
 * ESP32 AJAX rangefinder
 * Updates and Gets data from webpage without page refresh
 * Based on examples from Sparkfun and https://circuits4you.com
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

// Globals for sensor settings
int range;         // Latest reading
int roi;           // region of Intrest setting
int budget;        // reading time budget in ms. (bigger==more accuracy)
int interval;      // how often to start readings in ms

// Distance history for rolling average (UNUSED AT PRESENT)
//#define HISTORY_SIZE 10
//int history[HISTORY_SIZE];
//byte historySpot;

// Settings
bool enabled = true; // main enabled/disabled thing
String mode = "mid"; // range mode

// Wifi
#if __has_include("mywifi.h")
  // I keep my settings in a seperate header file that is not checked into git
  #include "mywifi.h"
#else
  // Leave as is to create an accesspoint, or set ACCESSPOINT 
  //  to false and supply your networks SSID and PASSWORD.
  #define ACCESSPOINT
  #define ACCESSPOINTIP 192,168,4,1
  #define ACCESSPOINTMASK 255,255,255,1
  const char* ssid = "VL53L0X-demo";
  const char* password = ""; // no password == very insecure, but very easy to demo
#endif

// Webserver
WebServer server(80);

// Distance Sensor
SFEVL53L1X distanceSensor;

/*  Wiring:  
 * 3v3 on ESP32 goes to VCC on CMJU-531
 * GND on ESP32 goes to GND on CMJU-531
 * D21 (GPIO21, I2C SDA) on ESP32 goes to SDA on CMJU-531
 * D22 (GPIO22, I2C SCL) on ESP32 goes to SCL on CMJU-531
 * D5 (GPIO5) on ESP32 goes to XSHUT on CMJU-531 (shutdown control, not currently used)
 * D18 (GPIO18) on ESP32 goes to GPIO1 on CMJU-531 (interrupt line, not currently used)
*/

// Uncomment the following lines if we start to use the shutdown and interrupt pins.
//#define SHUTDOWN_PIN 2
//#define INTERRUPT_PIN 3
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

// Lidar Servo - Not implemented; (uncomment and fill in settings)
// setup below assumes a H-bridge stepper driver for a small (5v Unipolar) motor
// Changes would be needed for a stepstick driver and a 'better' stepper, or a servo.

#define LIDAR
#ifdef LIDAR
  #include <Stepper.h>
  
  // Hardware based defines
  // Total steps/revolution for motor 
  // (5v geared motors are often multiples of 513 steps/rev)
  #define STEPS_PER_REV 2052 

  // a useful constant (STEPS_PER_REV / 360)
  #define STEPS_PER_DEGREE 5.7 

  // Stepper RPM (integer), my stepper maxes at 15rpm.
  int STEPPER_RPM = 12;
  
  // No zero/endstop support or hardware yet..
  // #define ENDSTOP_PIN unset
  
  // Soft endstops (180 degree sweep for demo unit)
  #define STEPS_MIN -513
  #define STEPS_MAX 513

  // Reverse motor movement (NOT implemented yet)
  // #define SERVO_INVERT false 

  // initialize the stepper library on (free) pins 33, 26, 25, 27
  Stepper lidarStepper(STEPS_PER_REV, 33, 26, 25, 27);

  int currentStep = 0; // Assume servo is at '0' to start.  



#endif

// Other
#define LED 2    // On-Board LED pin
#define BLINK 30 // On-Board LED blink time in ms


//===============================================================
// Setup
//===============================================================

void setup(void){
  // Misc. hardware
  pinMode(LED, OUTPUT);

#ifdef LIDAR
  lidarStepper.setSpeed(STEPPER_RPM);
#endif

  // Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  
  // Turn the LED on once serial begun
  digitalWrite(LED, HIGH);          

#if ACCESSPOINT
  // Access point 
  IPAddress ourIP(ACCESSPOINTIP);
  IPAddress ourMask(ACCESSPOINTMASK);
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ourIP, ourIP, ourMask);
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
  server.on("/range", handleRange);     // Update of distance and reading status
  server.on("/info", handleInfo);       // more detailed sensor info
  //server.on("/scan", handleScan);     // read a scan of data

  // Settings
  server.on("/near", handleNearMode);   // mode seting
  server.on("/mid", handleMidMode);     // mode seting
  server.on("/far", handleFarMode);     // mode seting
  server.on("/roiplus", handleRoiPlus);   // expand ROI
  server.on("/roiminus", handleRoiMinus); // reduce ROI
  server.on("/budgetplus", handleBudgetPlus);   // increase timing budget
  server.on("/budgetminus", handleBudgetMinus); // decrease timing budget
  server.on("/intervalplus", handleIntervalPlus);   // increase measurement interval
  server.on("/intervalminus", handleIntervalMinus); // decrease measurement interval

  // Commands
  server.on("/on", handleOn);           // Sensor Enable
  server.on("/off", handleOff);         // Sensor Disable
  #ifdef LIDAR
    server.on("/left", handleLeft);       // Step left
    server.on("/right", handleRight);     // Step right
  #endif
  
  // Start web server
  server.begin();
  Serial.println("HTTP server started");

  // Start sensor
  Wire.begin();
  delay(250);
  if (distanceSensor.begin() == true)
    Serial.println("Sensor online!");
   

  // read sensor initial values
  roi = distanceSensor.getROIX();
  budget = distanceSensor.getTimingBudgetInMs();
  
  // Clean history..
  //for (int x = 0 ; x < HISTORY_SIZE ; x++)
  //  history[x] = 0;

  // Setup complete, turn LED off
  digitalWrite(LED, LOW);   // turn the LED off now init is successful

  // sensor default mode, and start it if enabled by default
  handleMidMode();
  if (enabled == true) distanceSensor.startRanging();

}


//=========================================
// Handlers for responses to http requests
//=========================================

// Mainpage 

void handleRoot() {
  digitalWrite(LED, HIGH);          // blink the LED
  String s = MAIN_page;             //Read HTML from the MAIN_page progmem
  server.send(200, "text/html", s); //Send web page
  Serial.print("Sent the main page to: ");
  Serial.println(server.client().remoteIP().toString());
  delay(BLINK);
  digitalWrite(LED, LOW);
  delay(BLINK);
  digitalWrite(LED, HIGH);   // twice
  delay(BLINK);
  digitalWrite(LED, LOW);
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
  mode = "near"; 
  budget = 20;
  interval = budget+4; // from sensor datasheet; actual minimum is TimingBudget+4ms
  distanceSensor.setDistanceModeShort();
  distanceSensor.setTimingBudgetInMs(budget);
  distanceSensor.setIntermeasurementPeriod(interval);

  server.send(200, "text/plane", "near mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Near");
  delay(BLINK);
  digitalWrite(LED, LOW);
}

void handleMidMode()
{
  mode = "mid"; 
  budget = 33;
  interval = budget+4; // from sensor datasheet; actual minimum is TimingBudget+4ms
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(budget);
  distanceSensor.setIntermeasurementPeriod(interval);

  server.send(200, "text/plane", "mid mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Mid");
  delay(BLINK);
  digitalWrite(LED, LOW);
}

void handleFarMode()
{
  mode = "far"; 
  budget = 50;
  interval = budget+4; // from sensor datasheet; actual minimum is TimingBudget+4ms
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(budget);
  distanceSensor.setIntermeasurementPeriod(interval);

  server.send(200, "text/plane", "far mode");

  // blink LED and send to serial
  digitalWrite(LED, HIGH);
  Serial.println("Mode: Far");
  delay(BLINK);
  digitalWrite(LED, LOW);
}

#ifdef LIDAR
  void handleLeft()
  {
    int newStep = currentStep - (5 * STEPS_PER_DEGREE); 
    if (newStep < STEPS_MIN) stepTo(STEPS_MIN); else stepTo(newStep);
    server.send(200, "text/plane", "left");
  }
  
  void handleRight()
  {
    int newStep = currentStep + (5* STEPS_PER_DEGREE); 
    if (newStep > STEPS_MAX) stepTo(STEPS_MAX) ; else stepTo(newStep);
    server.send(200, "text/plane", "right");
  }
#endif

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

void handleBudgetPlus()
{
  int newbudget = budget + 10; // replace to step values from array
  if (newbudget > 500) budget = 500; else budget = newbudget;
  if (interval < (budget+4)) interval = budget + 4; // from sensor datasheet; actual minimum is TimingBudget+4ms
  server.send(200, "text/plane", "budgetplus");
  distanceSensor.setTimingBudgetInMs(budget);
  distanceSensor.setIntermeasurementPeriod(interval);
}

void handleBudgetMinus()
{
  int newbudget = budget - 10;// replace to step values from array
  if (newbudget < 20) budget = 20; else budget = newbudget;
  server.send(200, "text/plane", "budgetminus");
  distanceSensor.setTimingBudgetInMs(budget);
}

void handleIntervalPlus()
{
  int newinterval = interval + 10;
  if (newinterval > 1500) interval = 1500; else interval = newinterval;
  server.send(200, "text/plane", "intervalplus");
  distanceSensor.setIntermeasurementPeriod(interval);
}

void handleIntervalMinus()
{
  int newinterval = interval - 10;
  if (newinterval < (budget+4)) interval = budget + 4; // from sensor datasheet; actual minimum is TimingBudget+4ms
  else interval = newinterval;
  server.send(200, "text/plane", "intervalminus");
  distanceSensor.setIntermeasurementPeriod(interval);
}

// Now come the actual reading requests/handlers

void handleRange() {
  String out;
  StaticJsonDocument<50> range;
  if (enabled == true)
  {
    range["Distance"] = distanceSensor.getDistance();
    range["RangeStatus"] = distanceSensor.getRangeStatus();
    #ifdef LIDAR
      range["Angle"] = (currentStep / STEPS_PER_DEGREE);
    #endif
  }
  else
  {
    range["Distance"] = -1;
    range["RangeStatus"] = -1;
    #ifdef LIDAR
      range["Angle"] = (currentStep / STEPS_PER_DEGREE);
    #endif
  }
  serializeJsonPretty(range, out);
  server.send(200, "text/plane", out);
}

void handleInfo()
{
  String out;
  StaticJsonDocument<350> infostamp;
  infostamp["Mode"] = mode;
  infostamp["TimingBudgetInMs"] = distanceSensor.getTimingBudgetInMs();
  infostamp["IntermeasurementPeriod"] = distanceSensor.getIntermeasurementPeriod();
  infostamp["DistanceMode"] = distanceSensor.getDistanceMode();
  infostamp["SignalPerSpad"] = distanceSensor.getSignalPerSpad();
  infostamp["SpadNb"] = distanceSensor.getSpadNb();
  infostamp["AmbientRate"] = distanceSensor.getAmbientRate();
  infostamp["Offset"] = distanceSensor.getOffset();
  infostamp["SignalThreshold"] = distanceSensor.getSignalThreshold();
  infostamp["SigmaThreshold"] = distanceSensor.getSigmaThreshold();
  infostamp["XTalk"] = distanceSensor.getXTalk();
  infostamp["ThresholdWindow"] = distanceSensor.getDistanceThresholdWindow();
  infostamp["DistanceThresholdLow"] = distanceSensor.getDistanceThresholdLow();
  infostamp["DistanceThresholdHigh"] = distanceSensor.getDistanceThresholdHigh();
  infostamp["ROIX"] = distanceSensor.getROIX();
  infostamp["ROIY"] = distanceSensor.getROIY();
  #ifdef LIDAR 
    infostamp["HasServo"] = true;
    infostamp["Step"] = currentStep;
  #else
    infostamp["HasServo"] = false;
  #endif
  char id [8]; sprintf(id, "0x%x", distanceSensor.getSensorID());
  infostamp["ID"] = id;

  serializeJsonPretty(infostamp, out);
  server.send(200, "text/plane", out);
}

//====================================
// Other functions (eg stepper handlers)
//====================================

void stepTo(int target) {
   if ( currentStep != target ) { // only move if needed)
    delay(1); // DUMMYZ
   }
}


//====================================
// Main Loop (invokes client handler)
//====================================

void loop(void){
  server.handleClient();
  delay(1);
}
