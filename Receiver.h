/*
  File: Receiver.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Detects signals from 433Mhz wireless devices
  Detects a pulse train from the receiver using the specifed pins
  The data output of the receiver needs to be connected to both pinA and pinB
  pinA and pinB need to be interrupt capable pins

  Uses a hardware timer via an instance of TimerBase abstract class
  (more accurate than timing pulses with the micros() function;
  The use of hardware is optional, using a hardware we can only really use 1 instance of this class for each timer
  We can modify the processStateChange function to use the micros() function, but this is less accurate

  Refer to cpp file for function descriptions and more info
*/
#ifndef Receiver_h
#define Receiver_h

#include "Arduino.h"
#include <AvrSTL.h>
#include <vector>
#include "TimerBase.h"
using std::vector;

/*
  SAMPLESIZE needs to be enough to capture the pulsetrains
  most seem to be 50 pulses repeated between 3 and 6 times
  250 is about the max we can use with 2k of RAM (250 ints requires 500 bytes)
  using 300 can result in crash due to heap and stack collision
  serial port uses 186 bytes and additional space is required for extracting pulses
  from the buffer and comparing to stored pulsetrains etc. 
*/
#define SAMPLESIZE 250 

class Receiver
{
  public:
    // Constructor
    Receiver(TimerBase *timer, int pinA, int pinB, int ledPin);
    
    volatile int timings[SAMPLESIZE]; // This is our circular buffer
    volatile unsigned int pos;
    volatile unsigned long startTime;
    volatile unsigned long detectionStartTime;
    volatile unsigned long endTime;
    volatile unsigned int overflowCount;
    volatile unsigned long rfPulseCount;
    volatile unsigned long rfPulseTrainDuration;
    volatile byte startingState; // We need a tri state bool value here so use byte instead
    volatile byte prevState;
    
    static Receiver *_instance; // A pointer to the class instance so the ISR's can call into the class instance
    void configure();
    void startScanning();
    void stopScanning();
    unsigned long available(unsigned int ledOff, unsigned int ledOn); //intervals for the LED flash
    void getPulseTrain(vector<int16_t> *pulseTrain);
    void printDebug(HardwareSerial &port);
    
    // Destructor
    ~Receiver();

  private:
    TimerBase *_timer;
    int _ledPin;
    int _pinA;  // pin2 is INT0 on an Arduino Uno / 328p
    int _pinB;  // pin3 is INT1 on an Arduino Uno / 328p
    int _interruptNum1;
    int _interruptNum2;
    bool _ledstate;
    unsigned int _counter; // Used for tracking the led flash
    const int _rfStartPulseDuration; // minimum pulse width to detect start of rf pulse train (microseconds)
    const unsigned int _rfPulseCountMin; // minimum number of pulses between start and radio silence to be considered valid
    const unsigned int _rfPulseCountMax; // minimum number of pulses between start and radio silence to be considered valid
    const int _rfSilenceDuration; // minimum pulse width to detect radio silence signifying end of transmission (microseconds)
    volatile bool _pulseTrainStartDetected;
    volatile unsigned long _prevTime;
  
    void attachInterrupts();
    void resetIsrVariables();
    void resetPulseTrainCapture();
    void detachInterrupts();

    /*
      The two ISR's below need to be declared static
      We can now only use 1 instance of this class without it getting a bit ugly
      We could add more static _instance variables (eg. _instance1, _instance2 ...)
      and modify the class constructor to accept a number to indicate which instance to use...
     */
    static void handleInterruptRising();
    static void handleInterruptFalling();

    void processStateChange(bool edgeState);
};

#endif
