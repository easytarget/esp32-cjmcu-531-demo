# CMJU-531 and ESP32 Demo using a VL53L1X library and Ajax to display on a self-hosted site.
## A crude, slow(ish), low fidelity Lidar system
 
Designed (initially) for the commonly available VL53L1X (4 meter) IR Time Of Flight sensor from ST Microelectronics:
https://www.st.com/en/imaging-and-photonics-solutions/vl53l1x.html

Using this in can be as easy as connecting to the `VL53L1X-demo` wifi network (no password needed), and then browsing to `192.168.4.1` for the UI. Other WiFi settings can be used of course, see comments.

![The results](/docs/graph-panel.png)

The goal of this project is to prove the suitability of this sensor for a small-scale LIDAR scanner to be used in domestic automation; robot vaccum cleaners, security robots, monitoring systems and, in my case, a K9 'companion' robot I am working on as a long-term plan.
The VL53L1X is just one of a range of similar Time-Of-Flight sensors from ST Micro. This sketch should be easily adaptable for them all, and other manufacturers have their own sensors which may also be suitable.

### Requirements:
#### Hardware:`
* ESP32 DEV module connected to a suitable sensor module; any ESP 32 module should be suitable, but you may need to adapt pinouts.
* ESP development libs installed in a viable development environment: see https://github.com/espressif/arduino-esp32 
*  For the Arduino IDE the instructions are at  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.mdi 
* I use the CJMCU-531 sensor carrier, these are freely available online from AliExpress and the like. There are other VL53L1 carrier modules and boards available, all should work with this.
* For the LIDAR scan mechanism I use a 5V stepper motor with gearbox, plus the H-Bridge control board supplied with it. These are very common and can be found on AliExpress, Ebay, hobby electronics stores etc. for a couple of Euros.

#### Software:
The following libraries are not installed as standard in the Arduino IDE but are searchable and installable from within the library manager.
* Sparkfun VL53L1X library: https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
* ArduinoJson library (v1.6+): https://arduinojson.org/
* If you get an 'error: WebServer.h: No such file or directory' when compiling; make sure you have set your board type to 'ESP32Dev module' ;-)

### Sensor Libraries
Currently I use the Sparkfun library; but there is also one from Pololu: https://github.com/pololu/vl53l1x-arduino & https://www.pololu.com/product/3415/resources

Both of these are 'light' libs designed for ATMega arduinos (eg limited RAM, etc). The Official ST lib is larger, but more complete and (arguably) better supported. It is also somewhat crankily documented, and requires a sign-up at stmicro .com to access.

The ST lib itself is redistributable, although the detailed documentation is not.. I still plan to move to this; and will provide it with the codebase here.
See: https://www.st.com/en/ecosystems/x-cube-53l1a1.html#overview

### Pins:
1. **3v3** on ESP32 goes to **VCC** on CMJU-531
1. **GND** on ESP32 goes to **GND** on CMJU-531
1. **D21** (GPIO21, I2C SDA) on ESP32 goes to **SDA** on CMJU-531
1. **D22** (GPIO22, I2C SCL) on ESP32 goes to **SCL** on CMJU-531

Not currently used, but would be required for multi-sensor or low power sentinel use.

5. _**D5** (GPIO5) on ESP32 goes to **XSHUT** on CMJU-531 (shutdown control)_
5. _**D18** (GPIO18) on ESP32 goes to **GPIO1** on CMJU-531 (interrupt line)_

If you are using a H-bridge driven Servo the default pins (easily changable in code) are:

7. _**D27**_ (GPIO27), _**D25**_ (GPIO25), _**D26**_ (GPIO26), _**D33**_ (GPIO23) on ESP32 should go to the respective 1,2,3,4, inputs on the H-Bridge driver.
7. **5v** and **GND** on the ESP32 are used for the stepper power supply; this places a reasonable demand on the USB power supply. Lockups and disconnects associated with motor movement are undoubtedly caused by the USB supply dipping and causing brownouts, use a decent PSU and cable.

### Demo setup
The WebUI for the demo is optimised for mobile phone displays, I run the unit in AP mode and connect from my Mobile. This allows me to demo to others easily too. 
![The Prototype](/docs/lidar-rig.jpg)
![The results](/docs/status-panel.png)

### Future Plans
https://github.com/easytarget/esp32-cjmcu-531-demo/issues
* Move to the Official ST micro libs
* SMP: handle sensor+servo on different core than the web server.
* stop using GET requests and move to a streaming protocol.

### Issues
The scanning action is driven by successful sending of the data to the web client. Ti is affected by network timeouts, packet drops and delays, these will make the scan action intermittent and somewhat laggy.. This would be resolved by implementing the data stream/smp plans above.

I have noticed the CMJU-531 I2C comms seem to lock up infrequently, and needed a full power cycle to resume. This has only happened when I'm ireflashing, rebooting and developing on this; it seems stable when left in operation and not being 'messed' with. 

### Base/Stand/Bracket
I have iincluded model files I used for my development board (Sparkfun NodeMCU) and a paramatised SCAD file that can accomodate other MCU and Stepper driver board sizes by adjusting the relevant settings.

Two different sensor mounts are provided; the v2(taller) one also mounts the sensor at a slight angle to avoid rangefinding the edge of the table, an Issue I found in testing ;-)
![The results](/docs/lidar-stand.png)
