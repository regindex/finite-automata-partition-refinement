#ifndef DFA_HPP_
#define DFA_HPP_

#include "partition.hpp"

typedef std::pair<uint_t*,std::unordered_set<uint_t>*> comp_count;

/*
	node of the DFA
*/
struct node_dfa
{
	// out edges
	std::vector<uint_t> out;
	// pointer to partition	
	part* out_part;
	// edge pointers to counts
	std::vector<comp_count*> count;
};

// class for directed unidirectional unlabeled DFA
class DFA_unidirectional_out_unlabelled{

public:
	// empty constructor
	DFA_unidirectional_out_unlabelled(){}
	// initialize the empty DFA
	DFA_unidirectional_out_unlabelled(uint_t no_nodes){
		// set number of nodes
		nodes = no_nodes;
		edges = 0;
		// initialize DFA
		DFA.resize(nodes);
		// initialize counts
		counts.resize(no_nodes);
		for(uint_t i=0;i<no_nodes;++i)
		{
			counts[i] = new comp_count;
			counts[i]->first = new uint_t;
			counts[i]->second = new std::unordered_set<uint_t>;
			*counts[i]->first = 0;
		}
	}

	uint_t no_nodes(){ return nodes; }
	uint_t no_edges(){ return edges; }

	node_dfa* at(uint_t i){

		assert(i < nodes);
		// return pointer to node
		return &DFA[i];
	}

	void add_edge(uint_t origin, uint_t dest, int label, partition& P){

		// insert out edge
		DFA[origin].out.push_back(dest);
		// insert pointer to part
		if(DFA[dest].out_part == nullptr)
		{
			DFA[dest].out_part = P.give_part(label);
		}
		// insert pointer to count in the edge
		DFA[origin].count.push_back(counts[dest]);
		*counts[dest]->first += 1;
		counts[dest]->second->insert(origin);
		// increment edge number
		edges++;
	}

	void delete_counts()
	{
		counts.resize(0);
		counts.shrink_to_fit();
	}

private:
	// number of nodes in the DFA
	uint_t nodes;
	// number of edges in the DFA
	uint_t edges;
	// vector containing all edges
	std::vector<node_dfa> DFA;
	// tmp count vector
	//std::vector<uint_t*> counts;
	std::vector<comp_count*> counts;
}; 


#endif