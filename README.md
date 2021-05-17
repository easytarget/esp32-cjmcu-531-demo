[![CI Status](https://api.travis-ci.com/easytarget/esp32-cjmcu-531-demo.svg?branch=master)](https://travis-ci.com/github/easytarget/esp32-cjmcu-531-demo) `- Master branch status`

# CJMCU-531 and ESP32 Demo using a VL53L1X library and Ajax to display on a self-hosted site.
## A crude, slow(ish), low fidelity Lidar system
 
Designed (initially) for the commonly available VL53L1X (4 meter) IR Time Of Flight sensor from ST Microelectronics:
https://www.st.com/en/imaging-and-photonics-solutions/vl53l1x.html

Using this in can be as easy as connecting to the `VL53L1X-demo` wifi network (no password needed), and then browsing to `192.168.4.1` for the UI. Other WiFi settings can be used of course, see comments.

![The results](/docs/kitchen-scan.png)

The goal of this project is to prove the suitability of this sensor for a small-scale LIDAR scanner to be used in domestic automation; robot vaccum cleaners, security robots, monitoring systems and, in my case, a K9 'companion' robot I am working on as a long-term plan.

The VL53L1X is just one of a range of similar Time-Of-Flight sensors from ST Micro. This sketch should be easily adaptable for them all, and other manufacturers have their own sensors which may also be suitable.

I have been pleasently surprised that the results are better than I expected; the scan above is of my Kitchen; here is what it looks like in reality, the sensor is the little black dot in the scan, and the little red light in the photo.

![The Source](/docs/kitchen-reality.jpg)

### Requirements:
#### Hardware:`
* ESP32 DEV module connected to a suitable sensor module; any ESP 32 module should be suitable, but you may need to adapt pinouts.
* I use a CJMCU-531 sensor carrier module, these are freely available online from AliExpress and the like. There are other VL53L1 carrier modules and boards available, all should work with this.
* For the LIDAR scan mechanism I have a 5V stepper motor with gearbox, plus the H-Bridge control board supplied with it. These are very common and can be found on AliExpress, Ebay, hobby electronics stores etc. for a couple of Euros.

#### Software:
I use the standard Arduino IDE to develop and program the unit. You need to install the ESP development board definitions and libraries using the instructions at https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md

The following 3rd party libraries are needed. They are not installed as standard in the Arduino IDE but are searchable and installable from within the library manager.
* Sparkfun VL53L1X library: https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library/
* ArduinoJson library (v1.6+): https://arduinojson.org/

Select the dev board you are using from the boards manager list (normally the default generic ESP32 dev board entry, no special memory or other configuration needed)

By default the unit will create a passwordless AccessPoint named `VL53L1X-demo`, you can connect to that and browse to `http://192,168,4,1/` to see the demo. Alternatively rename the file `mywifi.sample.h` to `mywifi.h` and fill in your own details to connect to existing networks or make a more secure AP.

### A note on Sensor Libraries
Currently I use the Sparkfun library; but there is also one from Pololu: https://github.com/pololu/vl53l1x-arduino & https://www.pololu.com/product/3415/resources

Both of these are 'light' libs designed for ATMega arduinos (eg limited RAM, etc). The Official ST lib is larger, but more complete and (arguably) better supported. It is also somewhat crankily documented, and requires a sign-up at stmicro .com to access.

The ST lib itself is redistributable, although the detailed documentation is not.. I still plan to move to this; and will provide it with the codebase here.
See: https://www.st.com/en/ecosystems/x-cube-53l1a1.html#overview

### Construction

A rather minimalist guide here, I'm afraid. I have a more descriptive set of instructions, phopos and a parts list on the Hackaday page for this project:
https://hackaday.io/project/168327-cheap-and-cheerful-lidar-demo-and-dev-system

You need to follow the pinout guide here to connect the sensor and your development board together. Similarlarly for the stepper and it's H-Board driver card. And you need to maount the boards and sensor somehow. A simple design for 3d-printing with a 51x28mm 15 pin/side devboard is in the repo, as is a scadfile that can be adapted for other sizes of board(s), see line (#32)[https://github.com/easytarget/esp32-cjmcu-531-demo/blob/master/3dPrints/lidar-stand.scad#L32] for dimensioning.

#### Pins:
1. **3v3** on ESP32 goes to **VCC** on CJMCU-531
1. **GND** on ESP32 goes to **GND** on CJMCU-531
1. **D21** (GPIO21, I2C SDA) on ESP32 goes to **SDA** on CJMCU-531
1. **D22** (GPIO22, I2C SCL) on ESP32 goes to **SCL** on CJMCU-531

Not currently used, but would be required for multi-sensor or low power sentinel use.

5. _**D5** (GPIO5) on ESP32 goes to **XSHUT** on CJMCU-531 (shutdown control)_
5. _**D18** (GPIO18) on ESP32 goes to **GPIO1** on CJMCU-531 (interrupt line)_

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

#### Contributing
* Fork the repo
* Make a branch with a sensible nraeme in your forked repo and commit changes to that
* Make a PR in this repo against your remote fork and branch
* Explain what you are doing and don't be slapdash.. I hate random.

### Issues
The scanning action is driven by successful sending of the data to the web client. It is affected by network timeouts, packet drops and delays, these will make the scan action intermittent and somewhat laggy.. This would be resolved by implementing the data stream/smp plans above.

I have noticed the CJMCU-531 I2C comms seem to lock up infrequently, and needed a full power cycle to resume. This has only happened when I'm reflashing, rebooting and developing heavily; it seems stable when left in operation and not being 'messed' with. 

### Base/Stand/Bracket
I have included model files I used for my development board (JoyIT NodeMCU) and a paramatised SCAD file that can accomodate other MCU and Stepper driver board sizes by adjusting the relevant settings.

Two different sensor mounts are provided; the v2(taller) one also mounts the sensor at a slight angle to avoid rangefinding the edge of the table, I have found that for best results I need to stand the unit on a box or vase.
![The results](/docs/lidar-stand.png)

### EndGoal:
Use a ESP32-CAM instead of the dev board, stream video, take pictures, record ranges all in one.

Then link that via Serial and level converters to a ATMega328 based robot chassis I already have; and go for a WiFi drive.
![The results](/docs/robot.jpg)
