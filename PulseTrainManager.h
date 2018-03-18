/*
  File: PulseTrainManager.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Retrieves pulse trains (arrays of ints) from Flash memory
  Refer to cpp file for function descriptions and more info
*/
#ifndef PulseTrainManager_h
#define PulseTrainManager_h

#include "Arduino.h"
#include <AvrSTL.h>
#include <vector>
#include <avr/pgmspace.h>
using std::vector;

class PulseTrainManager
{
  public:
    // Default Constructor
    PulseTrainManager();
    bool findPulseTrain(vector<int16_t> *detectedPulseTrain, char (&key)[6]);
    vector<int16_t> get(char (&key)[6]); // name is passed by reference, 5 chars + nul terminator
    // Destructor
    ~PulseTrainManager();

  private:
    int getSizeOfLargestPulseTrain();
    void readProgMem(int16_t *location, int size, vector<int16_t> *pulseTrain);
};

#endif