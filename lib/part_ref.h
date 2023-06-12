#ifndef PART_REF_H
#define PART_REF_H

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <cassert>

#include "../internal/Automaton.hpp"

typedef NFA_unidirectional_out_unlabelled graph; 

/**
 * @brief Sort a Wheeler NFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA input consistent NFA to sort
 */
void partition_refinement_NFA(partition& P, graph& NFA);

/**
 * @brief Prune an input consistent DFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param DFA input consistent DFA to sort
 */
void partition_refinement_DFA(partition& P, graph& DFA);


#endif /* PART_REF_H */
