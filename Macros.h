/*
  File: Macros.h
  Author: Chris Claxton 2018
  Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
  
  Description: Commonly used bits of code for showing info and debugging etc.
*/
#ifndef Macros_h
#define Macros_h

#include "Arduino.h"
#include "MemoryInfo.h"
#include "Streaming.h"
using namespace MemoryInfo;

extern HardwareSerial &sout;

#define PRINT_COMPILE_INFO \
sout << F("Compilation date: ") << F(__DATE__) << F(" ") << F(__TIME__) << endl;

// Macro for printing some Info
#define PRINT_MEM_INFO printMemoryInfo();
inline void printMemoryInfo() {
  sout << F("RAM/Data Start: ") << dataStart() << F(" (0x") << _HEX(dataStart()) << F(")") << endl;
  sout << F("HeapStart: ") << heapStart() << F(" (0x") << _HEX(heapStart()) << F(")") << endl;
  sout << F("HeapEnd: ") << heapEnd() << F(" (0x") << _HEX(heapEnd()) << F(")") << endl;
  sout << F("RAM End: ") << ramEnd() << F(" (0x") << _HEX(ramEnd()) << F(")") << endl;
  sout << F("DataSize: ") << dataSize() << F(" Bytes") << endl;
}

/* 
  Macro for printing MemoryInfo
  Note that the Stack pointer will be affected by calling the functions which obtain the memory info
*/
#define PRINT_MEM printMemoryUsage();
inline void printMemoryUsage() {
  sout << F("HeapSize: ") << heapSize() << F(" HeapFree: ") << heapFree() << F(" StackSize: ") << stackSize() << F(" SP: ") << stackPointer() << F(" (0x") << _HEX(stackPointer()) << F(")") << endl;
  sout << F("HeapAvail: ") << heapAvailable() << F(" Bytes") << F(" StackAvail: ") << stackAvailable() << F(" Bytes") << endl;
}

#endif