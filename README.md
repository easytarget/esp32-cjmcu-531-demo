# CMJU-531 and ESP32 Demo using a VL53L0X library and Ajax to display on a self-hosted site.
#### Requirements:
* Sparkfun VL53L0X library: (search for it in the arduino IDE or) get from https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
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
