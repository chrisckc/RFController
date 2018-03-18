/*
  File: RFController.ino
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Receives, matches and transmits pulse trains using 433Mhz RF hardware
  Used for listening to and controlling common 433Mhz RF equipment
  Such as door bells, wireless mains sockets etc.
*/
#include "Arduino.h"
#include <AvrSTL.h>
#include <vector>
#include "Macros.h"
#include "Transmitter.h"
#include "PulseTrainManager.h"
#include "SerialHelper.h"
#include "ProgMemGlobals.h"
#include "Receiver.h"
#include "Timer2.h"
#include "MemoryInfo.h"
#include "Vcc.h"
#include "Streaming.h"
using std::vector;
using namespace SerialHelper;
using namespace MemoryInfo;

// Uncomment this to enable debug mode (for capturing unknown pulsetrains)
//#define DEBUG 1
// Uncomment this to enable memory debug mode
//#define MEM_DEBUG 1

HardwareSerial &sout = Serial; //create an alias for the Serial class

// The number of times to transmit the pulse train (to overcome errors and interference)
static const byte repeatCount = 6; // repeat count of between 4 and 6 seems to be what most devices use
static const int ledPin = LED_BUILTIN; // use the built in LED on the aruduino board
// The pins connected to the Receiver module's data pin
static const int receiverPinA = 2;  // pin2 is INT0 on an Arduino Uno / 328p
static const int receiverPinB = 3;  // pin3 is INT1 on an Arduino Uno / 328p
// The pin connected to the transmitter module's data pin
static const int outputPin = 4;
// These value control the flashing of the LED (defines an 'on' duration within a range 0 to 65536)
static const unsigned int ledOn = 1;
static const unsigned int ledOff = 10000;
Timer2 timer2;
Receiver receiver(&timer2, receiverPinA, receiverPinB, ledPin);
static const int initialPulse = 6674;
static Transmitter transmitter(outputPin, initialPulse);
static PulseTrainManager pulseTrainManager;
#define CMDBUFFER_SIZE 6 // Serial input buffer to hold 5 character pulsetrain identifier keys
static char cmdBuffer[CMDBUFFER_SIZE];
vector<int16_t> *detectedPulseTrain;
const float VccCorrection = 5.0/5.0;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);
/* Define a structure with bit fields */
static struct {
   unsigned int serialDataReceived : 1;
   unsigned int pulseTrainReceived : 1;
} event;


void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
  while (!Serial);  // Wait for serial port to connect. Needed for native USB
  Serial.println();
  // Configures the Receiver and timer ready for scanning
  receiver.configure();
  // The supply voltage needs to be close to 5v for the 433Mhz recevier to work properly
  float supplyVoltage = vcc.Read_Volts();
  PRINT_COMPILE_INFO
  #ifdef MEM_DEBUG
    PRINT_MEM_INFO
    PRINT_MEM
    sout << endl;
  #endif
  sout << F("VCC: ") << supplyVoltage << F(" Volts") << endl;
  sout << F("PulseTrain Array Size: ") << pulseTrainArraySize << endl;

  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  // Start listening for pulseTrain...
  Serial.println(F("RF Controller ready"));
  Serial.println(F("scanning..."));
  receiver.startScanning();
}

void loop() {
  // Set the bit fields in the struct depending on which data is available
  event.pulseTrainReceived = (receiver.available(ledOff, ledOn ) > 0);
  event.serialDataReceived = (readLine(Serial.read(), cmdBuffer, CMDBUFFER_SIZE) > 0);

  if (event.pulseTrainReceived) {
    digitalWrite(ledPin, HIGH);
    detectedPulseTrain = new vector<int16_t>; // Allocated on the heap
    receiver.getPulseTrain(detectedPulseTrain);
    #ifdef MEM_DEBUG
      sout << F("detectedPulseTrain size: ") << detectedPulseTrain->size() << endl;
      sout << F("Address of detectedPulseTrain: ") << (int)detectedPulseTrain << F(" (0x") << _HEX((int)detectedPulseTrain) << F(")") << endl;           
      PRINT_MEM
    #endif
    char key[6];
    bool found = pulseTrainManager.findPulseTrain(detectedPulseTrain, key);
    // Display the result
    if (found) {
      // Display the code
      Serial.print(F("KEY: "));Serial.println(key);
    } else {
      // Print the pulse train if in debug mode
      #ifdef DEBUG
        receiver.printDebug(Serial);
        sout << endl;
      #endif
    }
    delete detectedPulseTrain; // Delete the allocated memory
    // Start listening again...
    #ifdef MEM_DEBUG
      PRINT_MEM
      sout << endl;
    #endif
    receiver.startScanning();
  }

  if (event.serialDataReceived) {
    digitalWrite(ledPin, HIGH);
    sout << F("CMD: ") << cmdBuffer << endl;
    vector<int16_t> *pulseTrain = &(pulseTrainManager.get(cmdBuffer));
    if (pulseTrain->size() > 0) {
      #ifdef MEM_DEBUG
        PRINT_MEM
        sout << F("Sending pulse train...") << endl;
      #endif
      // The recevier needs to be stopped to prevent receiving the pulseTrain that is about to be sent
      receiver.stopScanning();
      digitalWrite(ledPin, HIGH);
      transmitter.send(pulseTrain, repeatCount);
      digitalWrite(ledPin, LOW);
      Serial.println(F("OK"));
      // By using an explicit pointer for pulseTrain, the allocated memory is free'd by the time this point is reached
      // If an implicit const pointer was used, memory was not free'd until outside of the if block 
      #ifdef MEM_DEBUG
        PRINT_MEM
      #endif
      #ifdef DEBUG
        printStats(transmitter);
      #endif
      receiver.startScanning();
    } else {
      Serial.println(F("?")); // Indicate that the command / pulsetrain key was not recognised
    }
  }
  // Pause for debugging
  //while (true) {}
}

void printStats(Transmitter &transmitter) {
      sout << F("sent ") << transmitter.pulseCount << F(" pulses repeated ") << repeatCount <<  F(" times") << endl;
      sout << F("total duration ") << transmitter.totalDuration << F("us") << endl;
      sout << F("each pulse train sent in ") << transmitter.duration << F("us") << endl;
}

// Interrupt Service Routine (ISR) for when Timer2's counter overflows;
ISR(TIMER2_OVF_vect) // Timer2's counter has overflowed 
{
  timer2.incrementOverflowCounter(); // Increment the timer2 overflow counter
}