/*
  File: TimerBase.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Abstract Base class to be used by hardware timer classes
  provides a generic base class to implement a hardware timer class
  which uses one of the hardware timers available in the mcu
*/

#ifndef TimerBase_h
#define TimerBase_h

#include <Arduino.h>

class TimerBase
{
  public:
  // Constructor
  TimerBase::TimerBase() : _overflowCounter(0) {}

	virtual void configure() = 0;
	virtual unsigned long getCount() = 0;
	virtual void reset() = 0;
	virtual void unconfigure() = 0;
    // This needs to be called by the Timer overflow ISR which has to be globally defined
	virtual void incrementOverflowCounter();
	
  protected:
	volatile unsigned long _overflowCounter;
};

// Note: inline functions must be included in the header file
// Increment overflow counter (specify inline to improve performance)
inline void TimerBase::incrementOverflowCounter()
{
  _overflowCounter++;
}

#endif



