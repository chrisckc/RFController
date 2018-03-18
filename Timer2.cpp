/*
  File: Timer2.cpp
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 license

  This class uses the Arduino Timer2 hardware to provide a timing function accurate to 0.5us
  (the Arduino micros() function is only accurate to to 4us)

  Using this class will change the behavior of the PWM output when using the analogWrite function on Arduino Pins 3 & 11.  
  (PWM output on pins 9 & 10 is controlled by Timer1)
  It could also interfere with other libraries that make use of Timer2

  Timer 2 interrupts have a higher priority than Timers 0 and 1, which means that it takes precedence over the  millis() timer (Timer0) and Timer1
  To use this class you must implement the Timer2 overflow ISR in the main sketch
  and make it call the increment counter method in this class

  eg:
  ISR(TIMER2_OVF_vect)
  {
    timer2.incrementOverflowCounter(); //increment the timer2 overflow counter
  }
  Note that due to the use of the overflow interrupt, the call to getCount() may be delayed on some occasions.
  If the interrupt occurred just before the call was made, the timer value that is read may be slightly higher
  Testing with a uniform square wave shows that the delay occurs in only around every 100 reads or so.
  The maximum variation in timings was observed to be at the most 4 to 5 us.
  Here is a sample of the worst case of variation, notice the 36,-28 that was captured, which occurs around every 100 samples
  ,-32,32,-33,31,-32,32,-33,31,-32,32,-32,31,-32,32,-33,31,-32,32,-32,36,-28,31,-32,31,-32,31,-33,31,-32,32
  Using the 16bit Timer1 would allow the overflow to be avoided and timer resets used instead to eliminate this issue
  I wanted to leave Timer1 free for other uses and this variation is insignificant for the purpose this class is to be used for
*/
#include "Timer2.h"
#include <avr/io.h>


// Constructor
Timer2::Timer2() : TimerBase() {}

// Configure Timer2, this function must be called first for any of the other Timer2 functions to work.
void Timer2::configure()
{
  // Backup the Timer2 control registers
  _tccr2aBackup = TCCR2A;
  _tccr2bBackup = TCCR2B;

  TCCR2B = 0x00; // Disable Timer2 while its being configured
  
  TCNT2 = 0; // Reset the Timer2 counter
  TIFR2 = _BV(0);  // Correct method to reset the Timer2 overflow flag

  // Enable the Timer2 overflow interrupt
  // Bit 0 – TOIE: Timer/Counter2, Overflow Interrupt Enable
  TIMSK2 |= 0b00000001; // set bit zero to 1
  
  // Set Timer2 to normal operation mode, see ATMega328P datasheet.
  TCCR2A &= 0b11111100; // Set WGM20 and WGM21 to 0
  TCCR2B &= 0b11110111; // Set WGM22 to 0

  // Setup the Timer2 prescaler so the timer will increment every 0.5us (the timer will overflow every 128us)
  TCCR2B = TCCR2B & 0b11111000 | 0x02; 
}  
  
// Gets the total count for Timer2
unsigned long Timer2::getCount()
{
  // save the processor status register (grabs the current state of the global interrupt flag)
  uint8_t sreg = SREG;
  noInterrupts(); // disable interrupts while we read the timer and overflow flags
  uint8_t tVal = TCNT2; // Timer2 counter value
  boolean ovFlag = bitRead(TIFR2,0); // Timer2 overflow flag
  // If timer2 has overflowed since disabling interrupts handle it here
  if (ovFlag) {
    tVal = TCNT2; // re-read Timer2 value just incase it had not overflowed on previous read
    _overflowCounter++; 
    // Reset the Timer2 overflow flag to prevent the execution of the Timer2 overflow ISR
    // TIFR2 bit zero is the TOV flag, it is cleared by writing a logic 1 to the flag.
    // All flags in TIFR2 are cleared by writing a 1.
    // The explanation can be found here: http://www.microchip.com/webdoc/avrlibcreferencemanual/FAQ_1faq_intbits.html
    TIFR2 = _BV(0);  // correct method, _BV is just a macro for bit shifting _BV(n) is the same as (1 << n) 
    //TIFR2 |= 0b00000001; // wrong way to do it, using a read-modify-write operation can cause a race condition and clear other bits
    //TIFR2  = 0x01; // another valid way to clear the Timer2 Overflow Flag
    
  }
  SREG = sreg; // Restore SREG to it's previous state (also contains the global interrupt flag)
  //return _overflowCounter * 256UL + tVal; // Get total Timer2 count
  return (_overflowCounter << 8) + tVal; // Use a left shift by 8 instead of multiplication by 256
}

// Reset Timer2's counter TCNT2
void Timer2::reset()
{
  uint8_t sreg = SREG;
  noInterrupts(); // disable interrupts while we reset stuff
  TIFR2 = _BV(0);  //reset the Timer2 overflow flag;
  _overflowCounter = 0;
  TCNT2 = 0; //reset the Timer2 counter
  SREG = sreg; // restore SREG to previous state (contains the global interrupt flag)
}

// Undo the configuration changes made to Timer2
void Timer2::unconfigure()
{
  TIMSK2 &= 0b11111110; // disable the Timer2 overflow interrupt;
  TCCR2A = _tccr2aBackup; //restore default settings
  TCCR2B = _tccr2bBackup; //restore default settings
}

// Destructor
Timer2::~Timer2() {
  // nothing to destruct here
}
  
  


