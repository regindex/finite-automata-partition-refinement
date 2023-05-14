#ifndef PART_REF_H
#define PART_REF_H

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <cassert>

#include "../internal/NFA.hpp"
//#include "../internal/DFA.hpp"

typedef NFA_unidirectional_out_unlabelled graph; 

//typedef DFA_unidirectional_out_unlabelled dfa_graph; 

//typedef std::pair<uint_t*,std::unordered_set<uint_t>*> comp_count;

/**
 * @brief Sort a Wheeler NFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA input NFA to sort
 * @param V   verbosity on/off
 */
void partition_refinement_NFA(partition& P, graph& NFA);

/**
 * @brief Sort an input consistent DFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param DFA input consistent DFA to sort
 * @param n   number of nodes
 * @param K   alphabet size
 */
void partition_refinement_DFA(partition& P, graph& DFA);


#endif /* PART_REF_H */
