# Arduino/ESP32 Toy Project "Smart Greenhouse" / "Slimme Serre" (NL)

This is an IoT toy project that I made within the confounds of a "Digital Electronics" course I have taken. The ESP32 microcontroller used is "TTGO T-display ESP32". 


It was a fun little project to work on and the considerations regarding what matters on low power IoT devices/microcontrollers were also very interesting to consider for a change. 
Although as this project requires continious display updates to the main display & consistent internet connection so none of the low power features could _really_ be exploited here.


Temperature configuration is managed through the potentiometer, but some overriding actions (opening/closing window and tap) can be taken an online Adafruit.io board. The board also displays the current status of the "Greenhouse" and shows some basic history/statistics about previous days.

.

My takeaways from this toy project are that I'll probably continue messing with Arduino/IoT moving forward on my own time, even if it's not usually what I do in terms of work.

## Components

The components are mostly outlined in the diagram below, but some are not particularly clear. Here is a list based on names on the diagram:

- **BMP280** (Temperature/Pressure sensor -- it measures the "air temperature" for our "smart greenhouse" in this project)
- **Capacitors** (handmade ones -- they measure the "wetness" of the plant soil, as well as detect flowing water from the "tap" that is supposed to automatically water the plants in the "smart greenhouse" respectively. They work through "touch capsense" provided by the ESP32 microcontroller. The values were relatively measured to the setup)
- **Stepper Motor** (ICQUANZX 28BYJ-48 ULN2003 5V stepper motor + ULN2003 driver board for Arduino -- is supposed to "open/close" the tap to water the plants)
- **Servo Motor** (MMOBIEL SG90 9g Micro Servo Motor -- it is supposed to open the "window" to let air in when it gets too hot)
- **LDR** (GL5516 LDR -- it detects how much light the plants would be getting)
- **Potentiometer B5K** (Potentiometer B5K -- it allows you to control the "target temperature" manually)

Not mentioned on Diagram:

- **Switches/Buttons** (The "TTGO T-display ESP32" has built-in buttons that are used to detect whether the "door" is closed and the "window" is closed)

## Running

The project uses platform.io for Visual Studio Code.

It's required to have a file [./src/Credentials.h](./src/Credentials.h) and [./src/config.h](./src/config.h) in the `./src` folder.

Make sure this is added before building the project. You can copy and fill in [./src/Credentials.h.default](./src/Credentials.h.default)
and/or [./src/config.h.default](./src/config.h.default) respectively.

### Diagram

![Project Diagram](./slimme_serre_diagram.jpg "Project Diagram")

### License
MIT