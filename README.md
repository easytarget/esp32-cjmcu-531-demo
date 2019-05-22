# CMJU-531 and ESP32 Demo using a VL53L0X library and Ajax to display on a self-hosted site.
Connect to the 'VL53L0X-demo' wifi network, no password. then browse to 192.168.4.1
#### Requirements:
* ESP32 DEV module (adjust pinouts as needed for other ESP8266 modules)
* Sparkfun VL53L0X library: (search for it in the arduino IDE or) get from https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
* Arduino JSON library (v1.6+ I dont think this is 1.5 compatible)
#### Pins:
1. **3v3** on ESP32 goes to **VCC** on CMJU-531
1. **GND** on ESP32 goes to **GND** on CMJU-531
1. **D21** (GPIO21, I2C SDA) on ESP32 goes to **SDA** on CMJU-531
1. **D22** (GPIO22, I2C SCL) on ESP32 goes to **SCL** on CMJU-531
1. _**D5** (GPIO5) on ESP32 goes to **XSHUT** on CMJU-531 (shutdown control, optional?)_
1. _**D18** (GPIO18) on ESP32 goes to **GPIO1** on CMJU-531 (interrupt line, unused?)_
#### Shots
![The Prototype](/rangefinder-proto1.jpg)
![The results](/graph-panel.png) ![The results](/status-panel.png)
#### Plans
* Use one Ajax request instead of the current two.
* Landing page for AP mode (use dns/mdns, examples abound)
* Auto accesspoint selection from list (there is a lib for this)
* Fewer Handler routines, grock the URL supplied
* Update rate control
* More setting adjustments, on a seperate collapsable panel.
* Add a servo (== lidar); add interrupt-driven motion control loop with homing (and accelleration control?)
* SMP: handle sensor+servo on different core than the web server.
* Use the official Library
#### Logging
```
Booting Sketch...
Access Point started: VL53L0X-demo:
AP Address: 192.168.4.1
HTTP server started
Mode: Mid
Sent the main page
Sent the main page
Mode: Near
Turning On
Mode: Mid
```
#### WIP..
I'm working on this...
