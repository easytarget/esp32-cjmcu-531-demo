# CMJU-531 and ESP32 Demo using a VL53L1X library and Ajax to display on a self-hosted site.
Designed (initially) for the VL53L1X (4 meter) sensor:
https://www.st.com/en/imaging-and-photonics-solutions/vl53l1x.html

Using this in can be as easy as connecting to the 'VL53L0X-demo' wifi network (no password needed), and then browsing to '192.168.4.1' for the UI.

![The results](/docs/graph-panel.png)

#### Requirements:
* ESP32 DEV module connected to a suitable sensor; any ESP 32 module should be suitable, but you may need to adapt pinouts.
* ESP development libs installed in a viable development environment: see https://github.com/espressif/arduino-esp32 
  For the Arduino IDE the instructions are at  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md

The following libraries are not installed as standard in the Arduino IDE but are searchable and installable from within the library manager.
* Sparkfun VL53L1X library: https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
* ArduinoJson library (v1.6+): https://arduinojson.org/
* If you get an 'error: WebServer.h: No such file or directory' when compiling; make sure you have set your board type to 'ESP32Dev module' ;-)

# Sensor Libraries
Currently I use the Sparkfun library; but there is also one from Pololu:

https://github.com/pololu/vl53l1x-arduino :: https://www.pololu.com/product/3415/resources

Both of these are 'light' libs designed for ATMega arduinos (eg limited RAM, etc). The Official ST lib is larger, but more complete and (arguably) better supported. It is also somewhat crankily documented, and requires a sign-up at stmicro .com to access.

The ST lib itself is redistributable, although the detailed documentation is not.. I still plan to move to this; and will provide it with the codebase here.
See: https://www.st.com/en/ecosystems/x-cube-53l1a1.html#overview

#### Pins:
1. **3v3** on ESP32 goes to **VCC** on CMJU-531
1. **GND** on ESP32 goes to **GND** on CMJU-531
1. **D21** (GPIO21, I2C SDA) on ESP32 goes to **SDA** on CMJU-531
1. **D22** (GPIO22, I2C SCL) on ESP32 goes to **SCL** on CMJU-531

Not currently used:

5. _**D5** (GPIO5) on ESP32 goes to **XSHUT** on CMJU-531 (shutdown control, not currently used)_
5. _**D18** (GPIO18) on ESP32 goes to **GPIO1** on CMJU-531 (interrupt line, not currently used)_

#### Demo setup
![The Prototype](/docs/lidar-rig.jpg)
![The results](/docs/status-panel.png)

#### Plans
https://github.com/easytarget/esp32-cjmcu-531-demo/issues
* Add a servo (== lidar); add interrupt-driven motion control loop with homing, canned movements (and accelleration control?).
* SMP: handle sensor+servo on different core than the web server.
* stop using GET requests and move to a streaming protocol.

#### Issues
* I have noticed the CMJU-531 I2C comms seem to lock up occasionally and needs a full power cycle to resume. This has only happened when I'm rebooting and developing on this; it seems stable when left in operation and not being 'messed' with. 

#### Base
Mounts the sensor at a slight angle to avoid rangefinding the edge of the table.
![The results](/docs/lidar-stand.png)
