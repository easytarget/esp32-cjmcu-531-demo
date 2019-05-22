# ESP32/VL53L0X Demo using Ajax to display on a self-hosted site.
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
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0018,len:4
load:0x3fff001c,len:1100
load:0x40078000,len:9232
load:0x40080400,len:6400
entry 0x400806a8

Booting Sketch...
Connecting to: Coffeeshop

Connected to Coffeeshop
IP address: 192.168.0.128
HTTP server started
Mode: Low
Sent the main page
Sent the main page
Mode: Low
Turning On
Mode: Mid
```
#### WIP..
I'm working on this...
