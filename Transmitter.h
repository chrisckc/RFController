/*
  File: Transmitter.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Transmits a pulse train out of the specified pin a specified number of times
  For controlling 433Mhz wireless devices

  Refer to the comments in the cpp file for more detail
*/
#ifndef Transmitter_h
#define Transmitter_h

#include "Arduino.h"
#include <AvrSTL.h>
#include <vector>

class Transmitter
{
  public:
    // Constructor
    Transmitter(int pin, int initialPulseDuration);
    unsigned int pulseCount; // length of pulse train that was last sent
    unsigned long duration; // microseconds to send each pulse train 
    unsigned long totalDuration; // microseconds to send all pulse trains (duration * repeatCount)
    void send(std::vector<int16_t> *pulses, byte repeatCount); // pulses is a pointer to a vector<int16_t> array
    ~Transmitter();

  private:
    int _pin;
    int _initialPulseDuration;
    unsigned long _startTime;
    unsigned long _endTime;
    void sendPulseTrain(std::vector<int16_t> *pulses);
};

#endif