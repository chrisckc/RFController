/*
  File: Transmitter.cpp
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 license
*/
#include "Transmitter.h"

/*
  Constructor
  @pin the pin which is connected to the transmitter data pin
  @initialPulseDuration the duration of an initial high pulse sent by the transmitter
  used to allow a receivers automatic gain control to adjust ready for the pulses
*/
Transmitter::Transmitter(int pin, int initialPulseDuration) : _startTime(0), _endTime(0) {
  _pin = pin;
  _initialPulseDuration = initialPulseDuration;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

/*
  @pulses the pulse train to be sent
  @repeatCount number of times to repeat the pulse train.
  devices usually send pulsetrains multiple times to account of transmission errors
  the initial pulse only needs to be sent once
*/
void Transmitter::send(std::vector<int16_t> *pulses, byte repeatCount) {
  pulseCount = (*pulses).size();
  digitalWrite(_pin, HIGH);
  delayMicroseconds(_initialPulseDuration);
  _startTime = micros();  // On 16 MHz Arduino boards, micros has a precision of 4us
  for (unsigned int i = 0; i < repeatCount; i++) {
    sendPulseTrain(pulses);
  }
  digitalWrite(_pin, LOW);
  _endTime = micros();
  totalDuration = _endTime - _startTime;
  duration = totalDuration / repeatCount;
}

/*
  @pulses the pulse train to be sent
  uses the delay function so this is blocking code
*/
void Transmitter::sendPulseTrain(std::vector<int16_t> *pulses) {
  for (unsigned int i = 0; i < pulseCount; i++) {
    int timing = (*pulses)[i];
    bool value =  (timing > 0);
    digitalWrite(_pin, value);
    int pulseLength = abs(timing);
    // this is only ever accurate to 4us due to the accuracy of the micros() function
    delayMicroseconds(pulseLength - 12); // subtract 10 uS to account for execution of the previous code
  }
}

// Destructor
Transmitter::~Transmitter() {
  // nothing to destruct here
}