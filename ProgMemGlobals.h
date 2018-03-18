/*
  File: ProgMemGlobals.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Declare all PROGMEM globals here for storage in flash, not to be loaded into SRAM
  Refer to cpp file for function descriptions and more info
*/

#include <avr/pgmspace.h>

#ifndef ProgMemGlobals_h
  #define ProgMemGlobals_h

/*
  Struct to hold the pulseTrain data
  key: 3 character identifier for the pulsetrain
  pulseTrainSize: size of the pulsetrain (number of elements in the array)
  pulseTrain: the array of int16_t representing the pulse lengths in microseconds
*/
  struct PulseTrainStruct {
    char key[6];
    int pulseTrainSize;
    int16_t *pulseTrain;
  };
  typedef struct PulseTrainStruct PulseTrainStruct;

  // Stores an array of PulseTrainStruct
  extern const PulseTrainStruct pulseTrainArray[] PROGMEM;
  extern int pulseTrainArraySize;

#endif
