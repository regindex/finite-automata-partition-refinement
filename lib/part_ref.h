#ifndef PART_REF_H
#define PART_REF_H

#include "internal/definitions.hpp"
#include "internal/finite_automaton.hpp"

typedef NFA_unidirectional_out_unlabelled graph; 

/**
 * @brief Sort a (Pseudo-)Wheeler automaton using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA a (Pseudo-)Wheeler automaton to sort
 */
void partition_refinement_Wheeler_automaton(partition& P, graph& NFA);

/**
 * @brief Prune an input consistent automaton using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param DFA input consistent automaton to sort
 */
void partition_refinement_pruning(partition& P, graph& DFA);


#endif /* PART_REF_H */
