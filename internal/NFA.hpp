#ifndef NFA_HPP_
#define NFA_HPP_

#include "partition.hpp"

/*
	node of the NFA
*/
struct node
{
	// out edges
	std::vector<uint_t> out;
	// pointer to partition	
	part* out_part;
	// edge pointers to counts
	std::vector<uint_t*> count;
};

// class for directed bidirectional unlabeled NFA
class NFA_unidirectional_out_unlabelled{

public:
	// empty constructor
	NFA_unidirectional_out_unlabelled(){}
	// constructor 
	NFA_unidirectional_out_unlabelled(uint_t no_nodes, std::vector<std::tuple<uint_t,uint_t,int>>& edge_list, partition& P){
		// set number of nodes
		nodes = no_nodes;
		edges = 0;
		// initialize DFA
		NFA.resize(nodes);
		// initialize counts
		counts.resize(no_nodes);
		for(uint_t i=0;i<no_nodes;++i)
		{
			counts[i] = new uint_t;
			*counts[i] = 0;
		}
		// iterate over edge list
		for(uint_t i=0;i<edge_list.size();++i)
  		{
    		int label =  seq_nt6_table[std::get<2>(edge_list[i])];
    		add_edge(std::get<0>(edge_list[i]),std::get<1>(edge_list[i]),label,P);
  		}
  		// delete pointers to initial parts
  		P.delete_spoint();
  		counts.resize(0); counts.shrink_to_fit();
	}
	// initialize the empty NFA
	NFA_unidirectional_out_unlabelled(uint_t no_nodes){
		// set number of nodes
		nodes = no_nodes;
		edges = 0;
		// initialize DFA
		NFA.resize(nodes);
		// initialize counts
		counts.resize(no_nodes);
		for(uint_t i=0;i<no_nodes;++i)
		{
			counts[i] = new uint_t;
			*counts[i] = 0;
		}
	}

	uint_t no_nodes(){ return nodes; }
	uint_t no_edges(){ return edges; }

	node* at(uint_t i){

		assert(i < nodes);
		// return pointer to node
		return &NFA[i];
	}

	void add_edge(uint_t origin, uint_t dest, int label, partition& P){

		// insert out edge
		NFA[origin].out.push_back(dest);
		// insert pointer to part
		if(NFA[dest].out_part == nullptr)
		{
			NFA[dest].out_part = P.give_part(label);
		}
		// insert pointer to count in the edge
		NFA[origin].count.push_back(counts[dest]);
		*counts[dest] += 1;
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
	std::vector<node> NFA;
	// tmp count vector
	std::vector<uint_t*> counts;
};

/*
	node of the NFA
*/
struct in_node
{
	// out edges
	std::vector<uint_t> in;
	// pointer to partition	
	char label;
};

// class for directed unidirectional unlabeled NFA
class NFA_unidirectional_in_unlabelled{

public:
	// empty constructor
	NFA_unidirectional_in_unlabelled(){}
	// initialize the empty NFA
	NFA_unidirectional_in_unlabelled(uint_t no_nodes)
	{
		// set number of nodes
		nodes = no_nodes;
		edges = 0;
		// initialize DFA
		NFA.resize(nodes);
	}

	uint_t no_nodes(){ return nodes; }
	uint_t no_edges(){ return edges; }

	in_node* at(uint_t i){

		assert(i < nodes);
		// return pointer to node
		return &NFA[i];
	}

	void add_edge(uint_t origin, uint_t dest, int label){

		// insert out edge
		NFA[dest].in.push_back(origin);
		// set node label
		NFA[dest].label = label;
		// increment edge number
		edges++;
	}

private:
	// number of nodes in the DFA
	uint_t nodes;
	// number of edges in the DFA
	uint_t edges;
	// vector containing all edges
	std::vector<in_node> NFA;
};


#endif