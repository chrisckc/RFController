/*
  File: PulseTrainManager.cpp
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
*/
#include "PulseTrainManager.h"
#include "ProgMemGlobals.h" // provides pulseTrainArray global
#include "MemoryInfo.h"
#include "Streaming.h"
#include "Macros.h"
using namespace MemoryInfo;

extern HardwareSerial &sout;

// Constructor
PulseTrainManager::PulseTrainManager() {}


/*
  Finds a matching pulsetrain from the pulseTrainArray
  Some memory debug was required while writing this to check the heap usage etc.
  @param detectedPulseTrain the pulsetrain we want to find a match for
  @key out parameter, the key that was matched
  @return true if match was found

  TODO: memory could be optimised further by comparing directly to progmem data rather than copying into RAM first.
  Searching through and matching against 19 stored pulsetrains takes about 25ms,
  so speed is not really an issue for the intended use.
*/
bool PulseTrainManager::findPulseTrain(vector<int16_t> *detectedPulseTrain, char (&key)[6])
{
    // Time this function's execution...
    unsigned long t1 = micros();
    int detectedPulseTrainSize = (*detectedPulseTrain).size();
    vector<int16_t> pulseTrain; //allocated on the stack
    int size = getSizeOfLargestPulseTrain();
    //sout << F("largest pulseTrain: ") << size << endl;           
    pulseTrain.reserve(size); // the actual storage used is allocated on the heap
    //sout << F("Address of pulseTrain: ") << (int)&pulseTrain << F(" (0x") << _HEX((int)&pulseTrain) << F(")") << endl;           
    // Testing using malloc and free
    //PulseTrainStruct *item = (PulseTrainStruct*)malloc(sizeof(PulseTrainStruct));
    PulseTrainStruct item;
    bool itemFound = false;
    //PRINT_MEM
    //sout << endl;
    for (int i = 0; i < pulseTrainArraySize; i++)
    {
        memcpy_P(&item, &pulseTrainArray[i], sizeof item);
        //sout << F("key: ") << item.key << endl;
        //sout << F("item.pulseTrainSize: ")<< item.pulseTrainSize << endl;
        //PRINT_MEM
        // The detected pulse trains always end with the radio silence pulse, so should always be bigger
        if (detectedPulseTrainSize > item.pulseTrainSize) {
            // read the pulse train into RAM
            readProgMem(item.pulseTrain, item.pulseTrainSize, &pulseTrain);
            //sout << F("pulseTrainSize: ")<< pulseTrain.size() << endl;
            //PRINT_MEM

            // start from the last item in the pulse train ending at the item at index 1 (ignoring index 0)
            // the item at index 0 is the sync gap so no need to check it
            // detectedPulseTrain starts at the last item - 1 (last item is always the radio silence pulse)
            // count upwards but invert the counter so we can easily start at the end of both PulseTrains
            int matchCounter = 0;
            int skipCounter = 0;
            for (int k = 1; k < item.pulseTrainSize;)
            {
                int pulse = pulseTrain[item.pulseTrainSize - k];
                int index = detectedPulseTrainSize - 1 - k - skipCounter;
                if (index > -1) {
                    int detectedPulse = (*detectedPulseTrain)[index];
                    //sout << F("pulse: ") << pulse << F(" detectedPulse: ") << detectedPulse << endl;
                    int tolerance = abs((detectedPulse * 10L) / 100);
                    //sout << F("tolerance: ") << tolerance << endl;
                    if (pulse < detectedPulse + tolerance && pulse > detectedPulse - tolerance) {
                        matchCounter++;
                        k++;
                    } else {
                        // If no match start searching pulseTrain again from the new position in detectedPulseTrain
                        skipCounter++;
                        matchCounter = 0;
                        k = 1;
                    }
                    // If the pulse train match has not started in the last (pulseTrainSize + 2) pulses, give up
                    if (skipCounter > item.pulseTrainSize + 2) {
                        break;
                    } 
                    //sout << F("matchCounter: ") << matchCounter << F("skipCounter: ") << skipCounter << endl;
                } else {
                    break; //detectedPulseTrain exhausted so exit
                }          
            }
            pulseTrain.clear(); //must make sure it is cleared for the next iteration
            //sout << F("pulseTrain cleared") << endl;
            //PRINT_MEM
            if (matchCounter > item.pulseTrainSize - 2) {
                itemFound = true;
                //sout << F("Match Found!") << endl;
                break;
            }
        }
        //sout << endl;
    }
    unsigned long time = micros() - t1;
    // This function takes ...... to search through a pulseTrainArraySize of 19 elements
    //sout << F("time: ") << time << endl;
    if (itemFound) {
        strcpy(key,item.key);
        return true;
    } else {
        return false;
    }
}

/*
  Private: Finds the largest pulsetrain in the pulseTrainArray
  @return the size as an int
*/
int PulseTrainManager::getSizeOfLargestPulseTrain()
{
    PulseTrainStruct item;
    int size = 0;
    for (int i = 0; i < pulseTrainArraySize; i++)
    {
        memcpy_P(&item, &pulseTrainArray[i], sizeof item);
        if (item.pulseTrainSize > size) size = item.pulseTrainSize;
    }
    return size;
}

/*
  Gets a pulsetrain from the pulseTrainArray based on its key
  @param key the 5 character char array (5 chars plus null terminator)
  @return an instance of vector<int16_t> containing the pulsetrain
*/
vector<int16_t> PulseTrainManager::get(char (&key)[6])
{
    // This function takes 192us to search through pulseTrainArraySize of 19 elements
    //unsigned long t1 = micros();
    vector<int16_t> pulseTrain;
    PulseTrainStruct item;
    for (int i = 0; i < pulseTrainArraySize; i++)
    {
        memcpy_P(&item, &pulseTrainArray[i], sizeof item);
        //Serial.println(item.key);
        if (strcmp(key, item.key) == 0) {
            pulseTrain.reserve(item.pulseTrainSize);
            readProgMem(item.pulseTrain, item.pulseTrainSize, &pulseTrain);
            break;
        }
    }
    //unsigned long time = micros() - t1;
    //Serial.println(time);
    return pulseTrain; // the compiler implements passing by reference for objects
}

/*
  Private: Reads a pulsetrain from the the program memory (flash)
  @param location a pointer to an int16_t array stored in progmem
  @size size of the array stored in progmem
  @pulsetrain out paramter, the vector<int16_t> to be filled with contents of the array from progmem
  @return an instance of vector<int16_t> containing the pulsetrain
*/
void PulseTrainManager::readProgMem(int16_t *location, int size, vector<int16_t> *pulseTrain) {
    for (int k = 0; k < size; k++)
    {
        int pulse = pgm_read_word_near(location + k);
        //Serial.println(pulse);
        pulseTrain->push_back(pulse);
    }
}

// Destructor
PulseTrainManager::~PulseTrainManager()
{
    // nothing to destruct here
}