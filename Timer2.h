/*
  File: Timer2.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
	
	This class uses Timer 2 to provide a timing function that is accurate to 0.5us
	(the Arduino micros() function is only accurate to to 4us)

	The TimerBase class can be used to creater other Timer classes
	If Timer1 is to be used, a class could easily be written for that
	and substituted in the Receiver class
	
	Refer to the comments in the cpp file for more detail
*/

#ifndef Timer2_h
#define Timer2_h

#include <Arduino.h>
#include "TimerBase.h"

class Timer2: public TimerBase
{
  public:
  // Defaut Constructor 
  Timer2();
   
	void configure(); 
	unsigned long getCount();
	void reset();
	void unconfigure();
	~Timer2();
	
  private:
	//volatile unsigned long _overflowCounter; // moved into base class
	
	byte _tccr2aBackup; // will be used to backup default settings
	byte _tccr2bBackup; // will be used to backup default settings
};

// Note: refactored and moved into base class
// Note: inline functions must be included in the header file
// Increment overflow counter (specify inline to improve performance)
// inline void Timer2::incrementOverflowCounter()
// {
//   _overflowCounter++;
// }

#endif



