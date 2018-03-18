/*
  File: Receiver.cpp
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 license
*/
#include "Receiver.h"
//#include <climits>
#include <limits.h>

// The static member declared in the header must be defined outside of any class method
Receiver *Receiver::_instance = NULL;

/*
  Constructor
  @timer instance of TimerBase class
  @pinA, @pinB the pins which are connected to the receiver's data output
  @ledPin the pin which the LED is connected to (high = on)
  @return true if match was found
*/
Receiver::Receiver(TimerBase *timer, int pinA, int pinB, int ledPin) :
            _timer(timer),
             _pinA(pinA),
             _pinB(pinB),
             _ledPin(ledPin),
             _rfStartPulseDuration(5000), // minimum pulse width to detect start of rf pulse train (microseconds)
             _rfPulseCountMin(25), // minimum number of pulses between start and radio silence to be considered valid
             _rfPulseCountMax(0), // minimum number of pulses between start and radio silence to be considered valid
             _rfSilenceDuration(20000) // minimum pulse width to detect radio silence signifying end of transmission (microseconds)
             
{}

/*
  This must be called before using the receiver
  configures the interrupts and the timer etc.
*/
void Receiver::configure()
{
  _ledstate = LOW;
  _counter = 0;
  resetIsrVariables();
  
  _interruptNum1 = digitalPinToInterrupt(_pinA);
  _interruptNum2 = digitalPinToInterrupt(_pinB);
  pinMode(_ledPin, OUTPUT);

  _timer->configure();
}

/*
  Checks if a pulse train is available
  Blinks the LED while we are waiting of pulses to be detected...
  Needs to be called repeatedly until the function returns a non zero result
  @ledOff the _counter value at which the LED is turned off
  @ledOn the _counter value at which the LED is turned on
  @return number of pulses detected
*/
unsigned long Receiver::available(unsigned int ledOff, unsigned int ledOn) {
  if (endTime == 0) {
    if (_counter == ledOff && _ledstate == HIGH) {
      _ledstate = LOW;
      digitalWrite(_ledPin, _ledstate);
    } else if (_counter == ledOn && _ledstate == LOW) {
      _ledstate = HIGH;
      digitalWrite(_ledPin, _ledstate);
    } else {
      _counter++; // _counter is allowed to overflow
    }
    return 0;
  } else {
    return rfPulseCount;
  }
}

/*
  Extracts the pulse train from the circular buffer in the correct order
  starts from the oldest entry in the buffer
  @pulseTrain out parameter, the pulsetrain extracted from the buffer
  An explicit pointer used for pulseTrain rather than a reference (&), this fits
  better with the intended use case where new is used to create the instance passed in.
  This function was changed to use an out parameter rather than return an object as
  some testing showed heap fragmentation despite using reserve
*/
void Receiver::getPulseTrain(vector<int16_t> *pulseTrain) {
  //vector<int16_t> pulseTrain; // old method
  if (overflowCount > 0) {
    pulseTrain->reserve(SAMPLESIZE); //pulseTrain.reserve(SAMPLESIZE);
  } else {
    pulseTrain->reserve(pos + 1);
  }
  unsigned int startIndex = 0;
  // if the butter has overflowed, start at the correct location in the buffer
  if (overflowCount > 0) startIndex = pos+1;
  // Print from startIndex to end of buffer
  for (unsigned int i = startIndex; i < SAMPLESIZE; i++) {
    int pulse = timings[i];
    if (pulse == 0) break;
    pulseTrain->push_back(pulse);
  }
  // If overflowed print from zero to startIndex
  if (overflowCount > 0) {
    for (unsigned int i = 0; i < startIndex; i++) {
      int pulse = timings[i];
      if (pulse == 0) break;
      pulseTrain->push_back(pulse);
    }
  }
  // old method
  //return pulseTrain; // the compiler implements passing by reference for objects
}

/*
  Prints the pulse train debug information to the specified serial port
  @port the serial port to use
*/
void Receiver::printDebug(HardwareSerial &port) {
  digitalWrite(_ledPin, HIGH);
  port.println();
  port.println(F("scan result:"));
  port.print(F("listening duration: ")); port.print(endTime - startTime);port.println(F(" ms"));
  port.print(F("detection duration: ")); port.print(endTime - detectionStartTime);port.println(F(" ms"));
  port.print(F("buffer overflow count: ")); port.println(overflowCount);

  unsigned int startIndex = 0;
  // if the butter has overflowed, start at the correct location in the buffer
  if (overflowCount > 0) startIndex = pos+1;
  port.print(F("buffer start index: ")); port.print(startIndex);port.print(F(" starting state: ")); port.println(startingState);
  port.print(F("pulse train count: ")); port.print(rfPulseCount);port.print(F(" pulse train duration: ")); port.print(rfPulseTrainDuration);port.println(F(" us"));
  port.println(F("pulse train buffer:"));
  // Print from startIndex to end of buffer
  for (unsigned int i = startIndex; i < SAMPLESIZE; i++) {
    //port.print( boolToDisplay(values[i]) );
    port.print( timings[i] );
    port.print(",");
  }
  // If overflowed print from zero to startIndex
  if (overflowCount > 0) {
    for (unsigned int i = 0; i < startIndex; i++) {
      //port.print( boolToDisplay(values[i]) );
      port.print( timings[i] );
      port.print(",");
    }
  }
  port.println();
  digitalWrite(_ledPin, LOW);
}


/*
  start scanning for pulses on the pins
*/
void Receiver::startScanning() {
  attachInterrupts();
}

/*
  stop scanning for pulses on the pins
*/
void Receiver::stopScanning() {
  detachInterrupts();
}

/*
  reset the variables and start scanning by configuring the interrupts
*/
void Receiver::attachInterrupts() {
  resetIsrVariables();
  _instance = this;
  startTime = millis();
  //Attach the Interrupts
  attachInterrupt(_interruptNum1, handleInterruptRising, RISING);
  attachInterrupt(_interruptNum2, handleInterruptFalling, FALLING);
}

/*
  Used to reset the capture state ready for capturing a new pulsetrain
  Clears all variables used in the ISR's
*/
void Receiver::resetIsrVariables() {
  _prevTime = 0;
  endTime = 0;
  prevState = 255;
  _pulseTrainStartDetected = false;
  resetPulseTrainCapture();
}

/*
  Resets the variables used to store the pulse train info
*/
void Receiver::resetPulseTrainCapture() {
  // Clear all variables used in the ISR to track a pulse train
  startingState = 255;
  detectionStartTime = 0;
  pos = 0;
  memset((void*)timings, 0, sizeof(timings)); //clear the pulse array
  overflowCount = 0;
  rfPulseCount = 0;
  rfPulseTrainDuration = 0;
}


/*
  Stops the capture by disabling interrupts
  Marks the capture as ended by setting the endTime
*/
void Receiver::detachInterrupts() {
  detachInterrupt(_interruptNum1);
  detachInterrupt(_interruptNum2);
  if (endTime == 0) {
    endTime = millis();
  }
}

/* 
  The ISR which is called when the rising edge statechange occurs
  Declared static in the header file to make it work
*/
void Receiver::handleInterruptRising() {
  _instance->processStateChange(true);
}

/* 
  The ISR which is called when the falling edge statechange occurs
  Declared static in the header file to make it work
*/
void Receiver::handleInterruptFalling() {
  _instance->processStateChange(false);
}

/* 
  Common function to process the state changes detected in each ISR
  If edgeState = true we are processing a rising edge
  The minumum pulse width that can be detected by the interrupts is probably about 15us
  due to the time taken to enter and exit the ISR (about 5us) plus time taken to execute the code
  smaller pulse widths will be recorded as the minimum duration it takes to execute the code
*/
void Receiver::processStateChange(bool edgeState) {
  // protection from processing a pending interrupt if a flag was set just before detachInterrupts() was called
  if (endTime != 0) return;
  //const unsigned long time = micros(); // can use this instead of hardware timer, only accurate to nearest 4us
  const unsigned long time = _timer->getCount(); //timer is more accurate then using the micros function
  //Serial.println(time);
  if (_prevTime > 0) {
    unsigned long duration = (time - _prevTime) / 2;
    //Serial.println(_prevTime);
    //Serial.println(time);
    //Serial.println();
    // Detect rf start pulse, either high or low
    if ((_pulseTrainStartDetected == false) && (duration > _rfStartPulseDuration)) {
      _pulseTrainStartDetected = true;
      detectionStartTime = millis();
    }
    if (_pulseTrainStartDetected == true) {
      if (duration > INT_MAX) {
        //__INT_MAX__ is not defined in limits.h or config.h
        // as result it causes an intelliSense error when using INT_MAX which is defined as __INT_MAX__
        duration = INT_MAX;
      }
      // Detect rf silence (low pulse), ignore unless more than 2 pulses detected 
      if (edgeState && (duration > _rfSilenceDuration)) {
        if (rfPulseCount > _rfPulseCountMin && ((rfPulseCount < _rfPulseCountMax) || _rfPulseCountMax == 0)) {
          detachInterrupts(); // Valid pulse train detected
        } else {
          resetPulseTrainCapture();
          startTime = millis() - duration;
        }
      } 
      // if edgeState = true (rising edge detected) we are measuring the duration of a low pulse
      if (edgeState == true) {
        timings[pos] = -duration; //negate the pulse duration
      } else {
        timings[pos] = duration;
      }
      // the previous state was the inverse of the edge transition state
      prevState = !edgeState;
      if (startingState == 255) {
        startingState = !edgeState;
      }
      rfPulseCount++;
      rfPulseTrainDuration+= duration;
      if (endTime == 0) {
        pos++;
        if (pos > SAMPLESIZE - 1) {
          pos = 0;
          overflowCount++;
        }
      }
    }
  }
  _prevTime = time;
}

// Destructor
Receiver::~Receiver() {
  // nothing to destruct here
}
