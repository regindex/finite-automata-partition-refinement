#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdint>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>

#ifndef M64
    typedef uint32_t uint_t;
    typedef int32_t int_t;
    #define U_MAX   UINT32_MAX
    #define I_MAX   INT32_MAX
#else
    typedef uint64_t uint_t;
    typedef int64_t int_t;
    #define U_MAX   UINT64_MAX
    #define I_MAX   INT64_MAX
#endif

// DNA alfabeth
const static int sigma = 6;

// conversion table
const static unsigned char seq_nt6_table[128] = {
    0, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
    5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
    5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
    5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
    5, 1, 5, 2,  5, 5, 5, 3,  5, 5, 5, 5,  5, 5, 4, 5,
    5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
    5, 1, 5, 2,  5, 5, 5, 3,  5, 5, 5, 5,  5, 5, 4, 5,
    5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5
};

// inverted conversion table
const static unsigned char inv_seq_nt6_table[128] = {
    5, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 4, 0, 3,  0, 0, 0, 2,  0, 0, 0, 0,  0, 0, 1, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 4, 0, 3,  0, 0, 0, 2,  0, 0, 0, 0,  0, 0, 1, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0
};



#endif /* PART_REF_H */