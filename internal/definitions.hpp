#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdint>
#include <string>
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
    typedef int32_t  int_t;
    #define U_MAX    UINT32_MAX
    #define I_MAX    INT32_MAX
#else
    typedef uint64_t uint_t;
    typedef int64_t  int_t;
    #define U_MAX    UINT64_MAX
    #define I_MAX    INT64_MAX
#endif

// ASCII alphabet size + two dummy labels
const static int sigma_ascii = 130;

#endif /* PART_REF_H */