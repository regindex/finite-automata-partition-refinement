#ifndef PART_REF_H
#define PART_REF_H

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <cassert>

#include "../internal/NFA.hpp"

typedef NFA_unidirectional_out_unlabelled graph; 

/**
 * @brief Sort a Wheeler NFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA input NFA to sort
 * @param V   verbosity on/off
 */
void partition_refinement_NFA(partition& P, graph& NFA, bool V);


#endif /* PART_REF_H */
