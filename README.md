# RF Controller for Arduino

## Description

This project was written as part of a Home Automation project. The project allows an Arduino to be used as an RF Scanner and Transmitter when used with 433Mhz RF modules.

The purpose of the project is to control cheap wireless receiver devices such as mains switching socket adapters, light switches and doorbells etc. Signals from transmitting devices such as doorbell buttons and sensors etc. can also be monitored.

Commands can be sent to the serial port to replay previously captured and stored pulse trains for controlling each device.
Commands are outputted on the serial port when pulse trains are received and matched against any stored pulse trains.
This allows a device such as a Raspberry Pi to be connected to provide a web interface using NodeRed for example.

## Why

I originally tried RFLINK (http://www.rflink.nl/blog2/) but found that it relies on being able to decode the protocol used by the device. I found that some devices could not be decoded, preventing the pulses from being captured and replayed.

There is no need to decode the protocol to control a device, we just need to capture the stream of pulses sent by the controlling device for each action such as on / off, then replay it. The only downside is that more storage is required to store a complete pulse train description rather than a representation of the pulse train that has been decoded using a specific protocol. This is not an issue as an Arduino Uno/Nano (ATmega328p) has easily enough flash storage to store all of the pulse trains required for my purposes.

Another downside of using RFLINK is that it only runs on an Arduino Mega (ATmega1280) due to it's SRAM requirements, I wanted to be able to use a standard Arduino Uno / Nano (ATmega328P).

## Prerequisites

#### Hardware

The hardware was built using an Arduino Nano clone, an RXB6 receiver and an XD-FST FS1000A Transmitter.

The following wiring can be used:
http://www.rflink.nl/blog2/wiring

With the following changes:
  * RXB6 pin 7 (data) -> Arduino Nano pin 3 and pin 4
  * FS1000A pin 3 (data) -> Arduino Nano pin 4
  * FS1000A pin 2 (vcc) -> Arduino Nano vcc pin

Note: The FS1000A can be supplied with 12V for increased Tx power.
If using an alternative transmitter that requires exactly 5V and is sensitive to it's supply voltage it is important that you connect the transmitter's Vcc pin to the Arduino Vcc pin rather than using a data pin set to high as suggested in RFLINK documentation as there will be a slight voltage drop via the data pin.

#### Software

The following software was used on a Mac running OSX Sierra

* [Arduino IDE](https://www.arduino.cc/en/Main/Software/)
* [Visual Studio Code](https://code.visualstudio.com/)
* [Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino/)
* [C++ Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### Required Arduino / C / C++ Libraries

Arduino Supply Voltage Library:
https://github.com/Yveaux/Arduino_Vcc

Arduino Streaming Library:
http://arduiniana.org/libraries/streaming/
Or use the version hosted here with modifications to resolve VS Code intellisense errors:
https://github.com/chrisckc/Streaming

C++ Standard Template Library (converted into a library):
https://github.com/chrisckc/avr-stl  (forked from https://github.com/andysworkshop/avr-stl)

Memory Information Library:
https://github.com/chrisckc/MemoryInfo

## Installing

To install the Arduino libraries, cd into your Arduino/libraries folder and run:
```
git clone <url>
```
clone the avr-stl repository into an AvrSTL subfolder to match the path specified in the c_cpp_properties.json file
```
git clone https://github.com/chrisckc/avr-stl AvrSTL
```
Check that the paths in the c_cpp_properties.json file match the paths of your libraries.

If you are using a Windows machine, the paths of the Arduino avr libraries will need to be modified to suit the Arduino installation location.

## Getting Started

For setup purposes, debug mode needs to be enabled to capture and display pulses from unknown devices, the captured pulse trains are manually added to the code inside ProgMemGlobals.cpp and assigned with a key to allow triggering via serial port commands. The captured pulse trains need to be analysed by eye to pick out the pulse train pattern from any surrounding noise. This is actually quite easy as the pulse trains usually start and end with long low pulses and are usually repeated multiple times.
Note: Devices that use rolling codes cannot be controlled with this software, however those seem to be in the minority.


## Capturing Pulse Trains

To capture unknown pulse trains, uncomment the '#define DEBUG' line inside RFController.ino.
With the project running on your Arduino, activate the controller for the wireless device within range of the receiver module and you should see a stream of positive and negative numbers appear on the serial console (be sure to use 115200 baud), copy and paste the number stream into your favourite text editor and look for patterns.

Here is a typical pulse train pattern: (pulse lengths are in microseconds, negative values are low pulses)
```
-6674,229,-605,228,-607,227,-608,226,-608,637,-199,224,-611,635,-199,633,-202,611,-614,220,-614,632,-203,631,-205,217,-617,218,-617,218,-617,217,-617,218,-217,628,-206,627,-209,214,-619,627,-208,627,-209,624,-211,213,-618,213,-32767
```
You will find the same pulse train pattern repeated multiple times, separated by a low pulse of anything between around 5000 microseconds to 20,000 microseconds. In the above pattern the separator was -6674, just pick out a single pulse train starting with the seperator. The end of the transmission is marked by a longer low pulse which represents the radio silence that exists while the receiver increases it automatic gain control, after which point a lot of noise is detected.

Notice it ends with -32767, this is a long period of radio silence, longer than the maximum period that can be timed in microseconds with a 16bit signed number so it has been capped at 16bit INT_MAX. 
The Receiver class contains logic to detect possible transmissions by looking for periods of radio silence. Without this it would just waste CPU cycles in matching pulse trains and continuously output random noise induced pulse trains in debug mode.

Once you have extracted a single pulse train which will look similar to the above example (may contain more or less pulses) it needs to be entered into the ProgMemGlobals.cpp file as an array. 
Each set of pulse trains then need to be made into a set of pulsetrainStruct's to be stored in the pulse trainArray variable.
Each pulsetrainStruct has a member variable "key" which is a 5 character code to identify the pulse train, review the cpp file for examples of how this is done.

To replay the pulse trains, just type the key (case sensitive) into the serial console followed by the enter key.
When a pulse train is detected it is matched against the stored pulse trains and the key is outputted to the serial terminal.

## Notes
I have deliberately left behind commented out code to show debug functions, alternative methods and remind myself what i have previously used for any future improvements.

The pulse trains that I have captured and placed in the ProgMemGlobals.cpp have been modified from their original form for security reasons. The number of pulse trains that can be stored is limited by the available Flash storage, the length of any single pulse train is limited by the available SRAM, the maximum with the ATmega328p's 2K of SRAM is about 160 pulses. The length restriction on SRAM can be removed with further optimisations as commented in TODO items, however I have not yet found a device which uses more than 156 pulses, most use around 50.

## Security
The security of the 433MHz devices that I have used with this code is virtually non existent, there is only the element of proximity to prevent someone else from controlling your devices. Due to the short range of the signals and likelihood of someone with technical skills wanting to stand outside and sniff the signals to gain control of few lamps connected to wireless switches, it's not really a huge issue. I would certainly not use these unsecured 433MHz devices for any physical safety or security related functions such as heating, alarm control or security sensors etc. The main issue with any wireless device, secured or not, is signal jamming rendering them useless.


## Built With

* [VisualStudio Code](https://code.visualstudio.com/)

The code was built using Visual Studio Code with the Microsoft Arduino and C++ extension.
Some effort was put in to make the intellisense error detection work properly
The other option was to just disable it by setting "C_Cpp.errorSquiggles": "Disabled"

The code should also be capable of being built using the Arduino IDE

## Author

* **Chris Claxton** - [chrisckc](https://github.com/chrisckc)

## License

This project is licensed under the GNU GPLv3 License - see the [LICENCE.md](LICENCE.md) file for details.

## Acknowledgements

* The RFLINK project for information on various RF modules etc.
* Authors of the libraries used in this project
