/*
   File: ProgMemGlobals.cpp
   Author: Chris Claxton 2018
   Copyright (c) 2018 Chris Claxton subject to the GNU GPLv3 licence
   
   Declare all Globals here for storage in Flash (PROGMEM), not to be loaded into SRAM.

   Tagging a variable declaration with the PROGMEM mnemonic prevents an SRAM copy being made of it.
   Declaration of PROGMEM (flash memory) data has to be a global-level operation.
   We cannot wait until the code executes to declare a PROGMEM string, it would be too late by then.
   The declaration of PROGMEM data has to be done outside of any function or class.
*/

#include "ProgMemGlobals.h"

/* 
   The pulse trains need to start with a long enough high (on signal) for the device's receiver to adjust
   it's gain to filter out the noise, followed by a low (off signal) as captured in the debug output.
   The required initial high signal is not included in the pulse trains here, it is added by the Transmitter class,
   defined by the 'initialPulseDuration' parameter and is the same for all transmissions.
   After the pulse train ends there is a period of radio silence after which the
   receiver's gain ramps back up again introducing noise.

   The data types are all intentionally set to 'int16_t' to ensure 16bit on other hardware.
   The maximum pulse width that can be measured will be +- 32767 us which is more than enough.
   Array sizes not specified to make it easy paste in new pulse trains without counting the number of pulses
   and reducing the scope for error.
   Adding pulse trains to PROGMEM does not affect the amount of SRAM available the the rest of the program.
 */

//Energie Sockets
// Button 1 (off/on)
const int16_t pt_eng10[] PROGMEM = { -6674,229,-605,228,-230,227,-608,608,-608,637,-199,224,-611,635,-199,204,-202,607,-614,220,-614,632,-203,631,-205,217,-617,610,-617,610,-617,217,-617,610,-617,628,-206,207,-209,214,-619,207,-208,207,-209,624,-211,213,-618,213 };
const int16_t pt_eng11[] PROGMEM = { -6676,228,-606,229,-230,608,-609,608,-611,635,-199,223,-612,636,-199,204,-202,607,-614,607,-614,632,-203,631,-205,610,-617,217,-619,220,-614,217,-617,610,-617,628,-207,628,-207,215,-619,207,-208,626,-209,625,-210,624,-208,212 };
// Button 2
const int16_t pt_eng20[] PROGMEM = { -6676,228,-230,228,-608,227,-230,228,-611,204,-199,223,-613,634,-201,634,-202,219,-616,219,-616,630,-205,630,-206,217,-619,216,-619,607,-614,217,-619,215,-619,626,-209,625,-210,215,-619,214,-624,623,-210,624,-212,211,-621,211 };
const int16_t pt_eng21[] PROGMEM = { -6675,229,-606,228,-610,225,-608,608,-609,637,-198,225,-610,635,-201,204,-202,607,-614,607,-614,632,-203,631,-205,607,-615,217,-618,217,-618,217,-618,217,-618,628,-207,207,-208,215,-620,215,-620,625,-210,625,-211,624,-207,213 };
// Button 3
const int16_t pt_eng30[] PROGMEM = { -6676,231,-603,230,-230,231,-604,228,-610,636,-196,223,-614,635,-199,635,-204,219,-614,220,-615,635,-200,629,-206,610,-616,610,-617,216,-618,610,-615,220,-616,628,-207,628,-208,216,-618,628,-208,214,-620,626,-210,214,-617,215 };
const int16_t pt_eng31[] PROGMEM = { -6676,229,-605,229,-230,227,-230,225,-609,639,-198,223,-610,636,-201,632,-203,607,-615,219,-615,632,-203,631,-205,216,-618,217,-618,610,-617,610,-618,215,-619,207,-207,628,-209,214,-619,207,-210,214,-622,622,-212,625,-207,211 };
// Button 4
const int16_t pt_eng40[] PROGMEM = { -6672,232,-605,230,-601,232,-605,229,-230,640,-195,228,-608,637,-201,634,-200,222,-613,222,-612,636,-201,629,-206,219,-614,220,-615,610,-618,215,-619,219,-615,631,-205,207,-207,216,-620,610,-616,216,-619,207,-209,213,-617,214 };
const int16_t pt_eng41[] PROGMEM = { -6674,232,-604,229,-230,227,-605,227,-613,204,-198,224,-611,637,-198,204,-204,220,-613,607,-615,631,-203,631,-205,217,-618,216,-619,216,-618,215,-620,219,-617,626,-208,626,-210,213,-621,214,-621,213,-621,625,-213,620,-208,212 };
//All (off/on)
const int16_t pt_eng00[] PROGMEM = { -6674,230,-605,229,-606,229,-605,229,-605,641,-198,224,-609,637,-199,204,-201,223,-613,222,-613,632,-203,630,-206,610,-618,610,-617,610,-617,607,-613,217,-618,628,-207,207,-208,216,-619,628,-207,625,-211,213,-621,214,-620,212 };
const int16_t pt_eng01[] PROGMEM = { -6674,229,-230,228,-230,227,-608,608,-610,636,-199,223,-611,632,-203,204,-204,219,-614,607,-614,632,-203,630,-206,217,-618,217,-617,217,-619,216,-618,217,-618,207,-208,207,-209,213,-622,626,-209,624,-211,212,-623,623,-209,215 };

//EnergyEgg power strip (off/on)
const int16_t pt_egg10[] PROGMEM = { -13445,326,-897,323,-898,321,-901,908,-294,914,-916,914,-304,898,-301,298,-921,894,-305,890,-310,289,-931,289,-932,883,-315,286,-935,285,-940,874,-319,282,-938,281,-938,877,-322,875,-324,279,-941,873,-325,277,-944,276,-936,267 };
const int16_t pt_egg11[] PROGMEM = { -13451,326,-896,317,-901,317,-907,907,-292,914,-304,305,-304,897,-302,299,-921,893,-305,890,-312,288,-931,290,-930,883,-316,287,-935,283,-936,879,-319,283,-938,281,-937,878,-321,876,-321,874,-326,872,-326,275,-945,276,-937,265 };

//Lloytron Doorbell button
const int16_t pt_ldb11[] PROGMEM = { -6000,591,-191,578,-579,196,-579,196,-198,578,-198,586,-196,584,-198,581,-200,189,-586,190,-587,192,-584,189,-587,189,-587,198,-201,187,-590,187,-589,578,-202,578,-204,576,-204,184,-593,575,-207,181,-594,183,-592,185,-590,184 };

//Brown generic fob
// Button A
const int16_t pt_bgfba[] PROGMEM = { -13967,527,-1284,1426,-392,510,-1301,1413,-406,498,-578,1404,-417,488,-1322,1394,-425,481,-579,1387,-431,474,-1336,476,-1337,474,-1335,1382,-1345,470,-1342,1378,-440,1375,-444,1370,-448,459,-1352,458,-1352,1345,-1349,462,-1350,461,-1348,462,-1349,461 };
// Button B
const int16_t pt_bgfbb[] PROGMEM = { -13973,518,-1292,1420,-400,506,-1305,1408,-409,496,-578,1399,-420,485,-1325,1392,-426,480,-579,1387,-432,474,-1337,472,-1338,471,-1338,1380,-438,468,-1343,1376,-442,465,-1346,463,-1347,1374,-445,1369,-450,456,-1354,456,-1355,457,-1355,1344,-1352,459 };
// Button C
const int16_t pt_bgfbc[] PROGMEM = { -13977,515,-1295,1415,-399,500,-1311,1402,-414,489,-578,1395,-420,483,-1328,1388,-428,480,-579,1380,-1345,469,-1340,469,-1341,468,-1345,1372,-441,464,-1346,1371,-446,1345,-1349,1345,-1349,459,-1349,461,-1348,1369,-448,1364,-453,1344,-1355,453,-1357,452 };
// Button D
const int16_t pt_bgfbd[] PROGMEM = { -13970,517,-1294,1419,-398,507,-1304,1407,-410,494,-578,1400,-416,487,-1324,1393,-424,480,-579,1386,-431,475,-1335,475,-1335,474,-1335,1382,-434,470,-1339,1380,-437,467,-1342,467,-1342,468,-1342,469,-1340,468,-1341,469,-1340,1378,-439,1373,-443,463 };



const int sizeOfInt = sizeof(int16_t);
/*
  Create an array of PulseStructs populated with identifier keys, pulse train sizes and pointers to the above pulse trains
  This array of structs is stored in PROGMEM only, adding elements to it does not affect the amount of SRAM available
  The only SRAM used is by the variable itself which is just a pointer to the first element of the array stored in the Flash memory 
 */
const PulseTrainStruct pulseTrainArray[] PROGMEM = {
  { "ENG10", (sizeof pt_eng10)/sizeOfInt, (int16_t*)&pt_eng10},
  { "ENG11", (sizeof pt_eng11)/sizeOfInt, (int16_t*)&pt_eng11},
  { "ENG20", (sizeof pt_eng20)/sizeOfInt, (int16_t*)&pt_eng20},
  { "ENG21", (sizeof pt_eng21)/sizeOfInt, (int16_t*)&pt_eng21},
  { "ENG30", (sizeof pt_eng30)/sizeOfInt, (int16_t*)&pt_eng30},
  { "ENG31", (sizeof pt_eng31)/sizeOfInt, (int16_t*)&pt_eng31},
  { "ENG40", (sizeof pt_eng40)/sizeOfInt, (int16_t*)&pt_eng40},
  { "ENG41", (sizeof pt_eng41)/sizeOfInt, (int16_t*)&pt_eng41},
  { "ENG00", (sizeof pt_eng00)/sizeOfInt, (int16_t*)&pt_eng00},
  { "ENG01", (sizeof pt_eng01)/sizeOfInt, (int16_t*)&pt_eng01},

  { "EGG10", (sizeof pt_egg10)/sizeOfInt, (int16_t*)&pt_egg10},
  { "EGG10", (sizeof pt_egg11)/sizeOfInt, (int16_t*)&pt_egg11},

  { "LDB11", (sizeof pt_ldb11)/sizeOfInt, (int16_t*)&pt_ldb11},

  { "BGFBA", (sizeof pt_bgfba)/sizeOfInt, (int16_t*)&pt_bgfba},
  { "BGFBB", (sizeof pt_bgfbb)/sizeOfInt, (int16_t*)&pt_bgfbb},
  { "BGFBC", (sizeof pt_bgfbc)/sizeOfInt, (int16_t*)&pt_bgfbc},
  { "BGFBD", (sizeof pt_bgfbd)/sizeOfInt, (int16_t*)&pt_bgfbd}

};
// grab the resulting size of the array and store it for future use
int pulseTrainArraySize = (sizeof pulseTrainArray) / sizeof(PulseTrainStruct);

// Notes:
// Storing the pulse trains in PROGMEM is trading off performance and added complexity for much lower memory usage.
// Performance is not critical here but memory usage is when we only have 2kb to play with.
// We can use the key to lookup items by searching the array but it won't be as fast as using something like
// std::map<char, int16_t*> pulseTrainDict; but if we use that we won't be storing the keys in PROGMEM

// We could also use a 2 dimensional array of void pointers instead as represented below, but the array of structs method above is better
/*
       |0 (key pointer) | 1 (size pointer) | 2 (array pointer)
------------------------------------------------------
row 0  |
row 1  |
row 2  |
row 3  |
row 4  |
*/

//const char key_ang10[6] PROGMEM = "ENG10";
//int size_ang10 PROGMEM = sizeof(pt_eng10) / sizeof(uint16_t);
//const char key_ang11[6] PROGMEM = "ENG11";
//int size_ang11 PROGMEM = sizeof(pt_eng11) / sizeof(uint16_t);
// ........
// const int16_t* lookupTable[20][3] = {
//      {(char*)&key_ang10, &size_ang10, (int16_t*)&pt_ang10},
//      {(char*)&key_ang11, &size_ang11, (int16_t*)&pt_eng11}
//      {.......}
//};
