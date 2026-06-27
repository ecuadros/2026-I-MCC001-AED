#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
using namespace std;

// C style
// typedef int T;

// C++11 style
using TI = int;
using TD = double;
using TS = string;
using LSI = int;
using LSD = double;
using LSS = string;
using LSC = char;
using LSB = bool;
using LSL = long;

struct CharTraits {
    using key_type = LSC;
    using ObjIDType = LSL;
    using LSI = LSI;
    using LSB = LSB;
    using LSL = LSL;
    using LSS = LSS;
    using LSC = LSC;
    using LSD = LSD;
};

// XT must be 32bit integer in Windows and 64bit in Linux
#if defined(_WIN32) || defined(_WIN64)
    using XT = int;
#else
    using XT = long;
#endif

using Ref = long;

#endif // __TYPES_H__
