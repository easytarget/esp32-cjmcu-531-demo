/*
 * ESP32 AJAX Rangefinder and Lidar demo
 * 
 * Owen Carter (https://easytarget.org/)
 * 
 * Project home @
 * https://github.com/easytarget/esp32-cjmcu-531-demo/
 * 
 * Originally based on examples from Sparkfun and https://circuits4you.com
 */
 
// 3rd party libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>

// Currently using the SparkFun library for the sensor: 
// https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
#include <SparkFun_VL53L1X.h>

// Comment out to disable servo (lidar)
#define LIDAR

// Embedded web page (stored in progmem)
#include "index.h"    

// Sensor setup; see:
// https://learn.sparkfun.com/tutorials/qwiic-distance-sensor-vl53l1x-hookup-guide#library-overview
int range;                 // Latest reading
int budgetIndex = 4 ;      // reading time budgets in ms. (bigger==more accurate but slower)
int budgetValue[7] = {15,20,33,50,100,200,500}; // default 100ms
byte opticalCenter = 199; 

// Settings
bool enabled = true; // main enabled/disabled control
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

// Lidar Settings
// setup below assumes a H-bridge stepper driver for a small (5v Unipolar) motor
// Changes would be needed for a stepstick driver and a 'better' stepper, or a servo.
#ifdef LIDAR
  #include <Stepper.h>
  
  // Pins, edit this if not using suggested wiring
  byte stepPin[4] = {27,25,26,33};

  // Total steps/revolution/degree for motor 
  // (5v geared motors are often multiples of 513 steps/rev)
  #define STEPS_PER_REV 2052 
  #define STEPS_PER_DEG 5.7

  // Uncomment to reverse motor movement 
  //#define SERVO_INVERT

  // Stepper speed in RPM (integer), my stepper maxes at 15rpm.
  int STEPPER_RPM = 12;
  
  // No zero/endstop support or hardware yet..
  // #define ENDSTOP_PIN unset
  
  // Soft endstops (240 degree sweep for demo unit)
  #define STEPS_MIN -769
  #define STEPS_MAX 769

  // initialize the stepper library
  Stepper lidarStepper(STEPS_PER_REV, stepPin[0], stepPin[1], stepPin[2], stepPin[3]);

  int currentStep = 0;                   // Assume servo is at '0' to start.  
  int manualStep = (STEPS_PER_REV/24);    // close to 15 degrees initially
  bool stepperPwr = 0;                   // stepper power status
  bool stepState[4] = {LOW,LOW,LOW,LOW}; // pin state during power off
  int scanStep = (STEPS_PER_REV/120);    // steps per scan reading, defaults to ~3 degrees
  int scanControl = 0;                   // 0=off, -1=stepping left, 1=stepping right
  int scanMin = 0;                       // start of scan
  int scanMax = 0;                       // end of scan
#endif

// Other
#define LED 2    // On-Board LED pin
#define BLINK 30 // On-Board LED blink time in ms

#define BUTTON 0 // On-Board button, aka 'boot', manual home and power off, comment out to disable.


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
  server.on("/data", handleData);     // Update of distance and status

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
    server.on("/s-scan90", handleScan90);             // Start a 60degree scan
    server.on("/s-scanfull", handleScanFull);          // Start a full sweep scan
    server.on("/s-scanstop", handleScanStop);         // Stop Scanning
    server.on("/s-scanback", handleScanBack);         // Reverse Scanning
    server.on("/s-left", handleStepperLeft);          // Go left
    server.on("/s-home", handleStepperHome);          // Go to center
    server.on("/s-right", handleStepperRight);        // Go right
    server.on("/s-zero", handleStepperZero);          // Set current position as zero
    server.on("/s-off", handleStepperOff);            // power off
    server.on("/s-manualplus", handleManualStepPlus);   // increase manual delta
    server.on("/s-manualminus", handleManualStepMinus); // decrease manual delta
    server.on("/s-scanplus", handleScanStepPlus);     // increase scan delta
    server.on("/s-scanminus", handleScanStepMinus);   // decrease scan delta
  #endif
  
  // Start web server
  server.begin();
  Serial.println("HTTP server started");

  // Start sensor
  Wire.begin();
  delay(250);
  if (distanceSensor.begin() == 0)
    Serial.println("Sensor online!");

  // Setup complete, turn LED off
  digitalWrite(LED, LOW);   // turn the LED off now init is successful

  // Set the sensor to it's default mode, and start it if needed
  handleMidMode();
  if (enabled == true) distanceSensor.startRanging();
}

//=========================================
// Handlers for responses to http requests
//=========================================

// Mainpage 
void handleRoot() {
  digitalWrite(LED, HIGH);          // blink the LED
  String s = MAIN_page;             //Read HTML from progmem
  server.send(200, "text/html", s); //Send web page
  digitalWrite(LED, LOW);
  Serial.print("Sent the main page to: ");
  Serial.println(server.client().remoteIP().toString());
  delay(BLINK);
  digitalWrite(LED, HIGH);          // blink again
  delay(BLINK);
  digitalWrite(LED, LOW);
}
 

void handleOn()
{
  server.send(200, "text/plain", "sensor enabled");
  usernotify("Sensor Enabled");
  enabled = true;
  distanceSensor.startRanging();
}

void handleOff()
{
  server.send(200, "text/plain", "sensor disabled");
  usernotify("Sensor Disabled");
  enabled = false;
  distanceSensor.stopRanging();
}

void handleNearMode()
{
  server.send(200, "text/plain", "near mode");
  usernotify("Near Mode");
  mode = "near"; 
  budgetIndex = 1;
  distanceSensor.setDistanceModeShort();
  distanceSensor.setTimingBudgetInMs(budgetValue[budgetIndex]);
  // from sensor datasheet; minimum intermeasurement is TimingBudget+4ms
  distanceSensor.setIntermeasurementPeriod(budgetValue[budgetIndex] + 10);
}

void handleMidMode()
{
  server.send(200, "text/plain", "mid mode");
  usernotify("Mid Mode");
  mode = "mid"; 
  budgetIndex = 2;
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(budgetValue[budgetIndex]);
  // from sensor datasheet; minimum intermeasurement is TimingBudget+4ms
  distanceSensor.setIntermeasurementPeriod(budgetValue[budgetIndex] + 10);
}

void handleFarMode()
{
  server.send(200, "text/plain", "far mode");
  usernotify("Far Mode");
  mode = "far"; 
  budgetIndex = 4;
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(budgetValue[budgetIndex]);
  // from sensor datasheet; minimum intermeasurement is TimingBudget+4ms
  distanceSensor.setIntermeasurementPeriod(budgetValue[budgetIndex] + 10); 
}

void handleRoiPlus()
{
  server.send(200, "text/plain", "roi plus");
  usernotify("ROI plus"); 
  int newroi = distanceSensor.getROIX() + 1;
  if (newroi > 16) newroi = 16;
  distanceSensor.setROI(newroi, newroi, opticalCenter);
}

void handleRoiMinus()
{
  server.send(200, "text/plain", "roi minus");
  usernotify("ROI Minus");
  int newroi = distanceSensor.getROIX() - 1;
  if (newroi < 4) newroi = 4;
  distanceSensor.setROI(newroi, newroi, opticalCenter);
}

void handleBudgetPlus()
{
  server.send(200, "text/plain", "budget plus");
  usernotify("Budget Plus");
  int newIndex = budgetIndex + 1;
  if (newIndex > 6) return; else budgetIndex = newIndex;
  distanceSensor.setTimingBudgetInMs(budgetValue[budgetIndex]);
  if (distanceSensor.getIntermeasurementPeriod() < (budgetValue[newIndex] + 4)) // TimingBudget+4ms minimum
    distanceSensor.setIntermeasurementPeriod(budgetValue[newIndex] + 10);
}

void handleBudgetMinus()
{
  server.send(200, "text/plain", "budget minus");
  usernotify("Budget Minus");
  int newIndex = budgetIndex - 1;
  if (newIndex < 0) return; else budgetIndex = newIndex;
  distanceSensor.setTimingBudgetInMs(budgetValue[budgetIndex]);
}

void handleIntervalPlus()
{
  server.send(200, "text/plain", "interval plus");
  usernotify("Interval Plus");
  int newinterval = distanceSensor.getIntermeasurementPeriod() + 20;
  if (newinterval > 1500) return;
  distanceSensor.setIntermeasurementPeriod(newinterval);
}

void handleIntervalMinus()
{
  server.send(200, "text/plain", "interval minus");
  usernotify("Interval Minus");
  int newinterval = distanceSensor.getIntermeasurementPeriod() - 20;
  if (newinterval < (budgetValue[budgetIndex] + 4)) // TimingBudget+4ms minimum
    newinterval = budgetValue[budgetIndex] + 10; 
  distanceSensor.setIntermeasurementPeriod(newinterval);
}

#ifdef LIDAR
  void handleScan90()
  {
    server.send(200, "text/plain", "90 degree scanning started");
    usernotify("90 Degree Scanning Started");
    scanMin = (-STEPS_PER_REV/8);
    scanMax = (STEPS_PER_REV/8);
    stepTo(scanMin);
    scanControl = 1; // start at left scan position , moving right
  }
  
  void handleScanFull()
  {
    server.send(200, "text/plain", "Full scanning started");
    usernotify("Full Degree Scanning Started");
    scanMin = STEPS_MIN;
    scanMax = STEPS_MAX;
    stepTo(scanMin);
    scanControl = 1; // start at left scan position , moving right
  }
 
  void handleScanStop()
  {
    server.send(200, "text/plain", "scanning stopped");
    usernotify("Scanning Stopped");
    scanControl = 0; // simply stop, nothing more.
  }
  
  void handleScanBack()
  {
    server.send(200, "text/plain", "scan direction reversed");
    usernotify("Scan Direction Reversed");
    if (scanControl == 1) scanControl = -1;
    else if (scanControl == -1) scanControl = 1;
  }
  
  void handleStepperLeft()
  {
    server.send(200, "text/plain", "stepper left");
    usernotify("Stepper Left");
    int newStep = currentStep - manualStep; 
    if (newStep < STEPS_MIN) stepTo(STEPS_MIN); else stepTo(newStep);
  }
  
  void handleStepperRight()
  {
    server.send(200, "text/plain", "stepper right");
    usernotify("Stepper Right");
    int newStep = currentStep + manualStep; 
    if (newStep > STEPS_MAX) stepTo(STEPS_MAX) ; else stepTo(newStep);
  }
  
  void handleStepperHome()
  {
    server.send(200, "text/plain", "stepper home");
    usernotify("Stepper Home");
    if (currentStep != 0) stepTo(0);
    stepperOff();
  }
  
  void handleStepperZero()
  {
    server.send(200, "text/plain", "stepper zero");
    usernotify("Stepper Zero");
    currentStep = 0;
  }

  void handleStepperOff()
  {
    server.send(200, "text/plain", "stepper off");
    usernotify("Stepper Off");
    stepperOff();
  }

  void handleManualStepPlus()
  {
    server.send(200, "text/plain", "stepper manual delta plus");
    usernotify("Stepper Manual Delta Plus");
    int newdelta = manualStep + 10;
    if (newdelta < 285) manualStep = newdelta;
  }
  
  void handleManualStepMinus()
  {
    server.send(200, "text/plain", "stepper manual delta minus");
    usernotify("Stepper Manual Delta Minus");
    int newdelta = manualStep - 10;
    if (newdelta >= 1) manualStep = newdelta;
  }

  void handleScanStepPlus()
  {
    int newdelta;
    server.send(200, "text/plain", "stepper scan delta plus");
    usernotify("Stepper Scan Delta Plus");
    if (scanStep < 10) newdelta = scanStep + 1; else newdelta = scanStep + 3;
    if (newdelta < 120) scanStep = newdelta;
  }
  
  void handleScanStepMinus()
  {
    int newdelta;
    server.send(200, "text/plain", "stepper scan delta minus");
    usernotify("Stepper Scan Delta Minus");
    if (scanStep < 11) newdelta = scanStep - 1; else newdelta = scanStep - 3;
    if (newdelta >= 1) scanStep = newdelta;
  }
#endif

// Now come the reading request + handler

void handleData() {
  String out;
  StaticJsonDocument<450> response;
  if (enabled == true)
  {
    response["Distance"] = distanceSensor.getDistance();
    response["RangeStatus"] = distanceSensor.getRangeStatus();
  }
  else
  {
    response["Distance"] = -1;
    response["RangeStatus"] = -1;
  }
  response["Mode"] = mode;
  response["TimingBudgetInMs"] = distanceSensor.getTimingBudgetInMs();
  response["IntermeasurementPeriod"] = distanceSensor.getIntermeasurementPeriod();
  response["DistanceMode"] = distanceSensor.getDistanceMode();
  response["ROIX"] = distanceSensor.getROIX();
  response["ROIY"] = distanceSensor.getROIY();
  response["SignalPerSpad"] = distanceSensor.getSignalPerSpad();
  response["SpadNb"] = distanceSensor.getSpadNb();
  response["AmbientRate"] = distanceSensor.getAmbientRate();
  response["Offset"] = distanceSensor.getOffset();
  response["SignalThreshold"] = distanceSensor.getSignalThreshold();
  response["SigmaThreshold"] = distanceSensor.getSigmaThreshold();
  response["XTalk"] = distanceSensor.getXTalk();
  #ifdef LIDAR 
    response["Angle"] = roundf((currentStep * 10) / STEPS_PER_DEG)/10;
    response["HasServo"] = true;
    response["Step"] = currentStep;
    response["ScanStep"] = scanStep;
    response["ManualStep"] = manualStep;
  #else
    response["HasServo"] = false;
  #endif
  char id [8]; sprintf(id, "0x%x", distanceSensor.getSensorID());
  response["Sensor ID"] = id;
  
  serializeJsonPretty(response, out);
  server.send(200, "text/plain", out);

  // Move sensor in a sweep when scanning.
  #ifdef LIDAR
      if (enabled == true && scanControl == -1) {
            int newStep = currentStep - scanStep; 
            if (newStep < scanMin) {
              newStep = currentStep + scanStep;
              scanControl=1;
            }
            stepTo(newStep);
            delay(40); // let stepper settle
      }
      if (enabled == true && scanControl == 1) {
            int newStep = currentStep + scanStep; 
            if (newStep > scanMax) {
              newStep = currentStep - scanStep;
              scanControl=-1;
            }
            stepTo(newStep);
            delay(40); // let stepper settle
      }
  #endif
}

//====================================
// Other functions (eg stepper handlers)
//====================================

// flash led and dump a message to serial to log actions
void usernotify(char message[]) {
  digitalWrite(LED, HIGH);   
  Serial.println(message);
  delay(BLINK);
  digitalWrite(LED, LOW);
}

#ifdef LIDAR
  // Go to a target position and set that as current
  void stepTo(int target) {
     if ( currentStep != target ) {   // only move if needed
      if ( !stepperPwr ) stepperOn(); // wake stepper as needed
      #ifdef SERVO_INVERT
        lidarStepper.step(-(target-currentStep));
      #else
        lidarStepper.step((target-currentStep));
      #endif
      currentStep = target;
     }
  }

  // Disable the stepper, record pin state so we can recover exact position
  void stepperOff() {
    delay(40); // wait for motor to settle, it drifts otherwise
    for (byte p=0; p < 4; p++) 
    {
      stepState[p] = digitalRead(stepPin[p]);
      digitalWrite(stepPin[p],LOW);
    }
    scanControl = 0; // stop any in-progress scans
    stepperPwr = false;
  }

  // Enable the stepper, restore last pin state
  void stepperOn() {
    for (byte p=0; p < 4; p++) 
    {
      digitalWrite(stepPin[p],stepState[p]);
    }
    delay(20); // wait for motor to settle back into energised position
    stepperPwr = true;
  }
#endif

//====================================
// Main Loop (invokes client handler)
//====================================

void loop(void){
  server.handleClient();
  #ifdef BUTTON
    #ifdef LIDAR
      // Deal with the (optional) active-low 'boot' button on the board.
      if (digitalRead(BUTTON) == LOW ) {
        delay(50); // crude debounce
        if (digitalRead(BUTTON) == LOW) {
          // home and disable stepper
          stepTo(0);
          stepperOff();
          Serial.println("Homed via button");
          // wait for button release
          while(digitalRead(BUTTON) == LOW) delay(10);
        }
      }
    #endif
  #endif
  delay(1);
}
