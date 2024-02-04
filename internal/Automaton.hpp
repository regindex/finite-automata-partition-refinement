#ifndef NFA_HPP_
#define NFA_HPP_

#include "partition.hpp"

/* node of the NFA */
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

	void set_source(uint_t source_)
	{
		this->source = source_;
	}

	uint_t give_source()
	{
		return this->source;
	}

	std::vector<char>* give_labels()
	{
		return &this->labels;
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

	void add_dummy_state(partition& P){

		// add dummy node in the partition
		P.add_node(nodes, 126);
		// add dummy node in the automaton
		NFA.push_back(node());
		// create new count value
		counts.push_back(nullptr);
		counts[nodes] = new uint_t;
		*counts[nodes] = 1;
		// make the dummy state the new source state
		NFA[nodes].out = NFA[source].out;
		NFA[nodes].count = NFA[source].count;
		NFA[nodes].out_part = P.give_part(126);
		// connect the old source to the new one
		NFA[source].out = std::vector<uint_t>{nodes};
		NFA[source].count = std::vector<uint_t*>{counts[nodes]}; 
	}

	void add_label(uint_t dest, char label){

		if(labels.size() == 0)
			labels = std::vector<char>(nodes,0);

		labels[dest] = label;
	}

	void delete_counts()
	{
		counts.resize(0);
		counts.shrink_to_fit();
	}

	/*  */
	uint_t map_state_to_part(partition& P, bool pruned = false)
	{
		// get head of the partition
		part* curr = P.give_head();
		// set node counter
		uint_t cnt = 0;
		// iterate over all parts
		while(curr != nullptr)
		{	
			// interate over nodes in each part
			for (const auto& e: *curr->nodes)
			{
				if( pruned ){ remove_deleted_edges(e); }
				// add one count if the node has no edges
				if( NFA[e].count.size() == 0 ){ NFA[e].count.resize(1); }
				NFA[e].count[0] = new uint_t(cnt);
			}
			// increase counter
			cnt++;
			// stop when reaching the last state
			curr = curr->next;
		}

		return cnt;
	}

	/*
		function to print the resulting pruned automaton to file
	*/
	void to_output_pruned(std::string out)
	{
		// fix automaton if we added the dummy state
		if( nodes < NFA.size() )
			NFA[source] = NFA[nodes];

		// open output file 
		std::ofstream ofile;
		ofile.open(out); 
		// scan the graph
		for(uint_t i=0;i<nodes;++i)
		{
			for(uint_t j=0;j<NFA[i].out.size();++j)
			{
				if( *NFA[i].count[j] > 0 )
				{
					ofile << i << " " << labels[NFA[i].out[j]] << " " << NFA[i].out[j] << "\n";
				}
			}
		}
		// close output file
		ofile.close();
	}

	void to_output_Wheeler(std::string out, partition& P, bool compact = true)
	{
		// initialize and open output files
		std::ofstream ofile, L, In, Out;
		// initialize needed variables
		uint_t out_d = 0, n = 0, m = 0;
		std::vector<uint_t> in_d, in_l; 
		// mapping state to part
		std::vector<uint_t> mapping;
		// vector storing 
		if( compact )
		{
			L.open(out+".L"); In.open(out+".in"); Out.open(out+".out"); 
		}
		else
			ofile.open(out); 
		// compute mapping state -> part
		n = map_state_to_part(P);
		// init indegree vector or write no. states to file
		if( compact )
		{
			in_d = std::vector<uint_t>(n,0);
			in_l = std::vector<uint_t>(sigma_ascii,0);
		}
		else
			ofile << n << "\n";
		// set current part to head of partition
		part* curr = P.give_head();
		// iterate over partition and output automaton
		while(curr != nullptr)
		{	
			// set containing reached states
			std::unordered_set<uint_t> dest_map;
			// set outdegree to zero
			out_d = 0;
			// iterate over nodes in part
			for (const auto& e: *curr->nodes)
			{
				// beginning state
				uint_t b = *(NFA[e].count)[0];
				// iterate over outgoing edges
				for(uint_t i=0;i<NFA[e].out.size();++i)
				{
					// compute reached state and label
					uint_t o = NFA[e].out[i];				
					uint_t op = *(NFA[o].count)[0];
					// check if destination has already been reached
					if(dest_map.find(op) == dest_map.end())
					{
						// increase in and out degree counter or write edge
						if( compact )
							{
								out_d++; in_d[op]++; in_l[labels[o]]++;
								L << labels[o]; 
							}
						else
							ofile << b << " " << labels[o] << " " << op << "\n";
						// increase edges count
						m++;
						// update set
						dest_map.insert(op);
					}
				}
			}
			// write out degree if needed
			if( compact )
				Out << out_d << "\n";
			// stop when reaching the last state
			curr = curr->next;
		}
		// close output files
		if( compact )
		{
			// write in degrees
			for(uint_t i=0;i<n;++i)
				In << in_d[i] << "\n";
			// close streams
			L.close(); In.close(); Out.close(); 
		}
		else
			ofile.close(); 
	}

	void remove_deleted_edges(uint_t e)
	{
		// remove an edge if its count is equal to zero
		for(uint_t i=0;i<NFA[e].out.size();++i)
		{
			if(*NFA[e].count[i] == 0){ NFA[e].out[i] = U_MAX; }
		}
	}

	/*
		function to print the resulting pruned automaton to file
	*/
	void to_output_pruned_Wheeler(std::string out, partition& P, bool suprema, bool compact = true)
	{
		// initialize and open output files
		std::ofstream ofile, L, In, Out;
		// initialize needed variables
		uint_t out_d = 0, n = 0, m = 0;
		std::vector<uint_t> in_d;
		// mapping state to part
		std::vector<uint_t> mapping;
		mapping.resize(nodes);
		// vector storing 
		if( compact )
		{
			L.open(out+".L"); In.open(out+".in"); Out.open(out+".out"); 
		}
		else
			ofile.open(out); 
		// compute mapping state -> part
		n = map_state_to_part(P,true);
		if( suprema ){ n--; }
		// init indegree vector or write no. states to file
		if( compact ){ in_d = std::vector<uint_t>(n,0); }
		else
			ofile << n << "\n";
		// set current part to head of partition
		part* curr = P.give_head();
		if( suprema )
		{
			curr->next->prev = nullptr;
			/* curr = curr->next; */
			curr = NFA[nodes].out_part;
		}
		// iterate over partition and output automaton
		while(curr != nullptr)
		{	
			// set containing reached states
			std::unordered_set<uint_t> dest_map;
			// set outdegree to zero
			out_d = 0;
			// iterate over nodes in part
			for (const auto& e: *curr->nodes)
			{
				//std::cout << e << std::endl;
				// beginning state
				uint_t b = *(NFA[e].count)[0];
				// iterate over outgoing edges
				for(uint_t i=0;i<NFA[e].out.size();++i)
				{
					// compute reached state and label
					uint_t o = NFA[e].out[i];
					// proceed in case of a non deleted edge
					if( o != U_MAX )
					{
						uint_t op = *(NFA[o].count)[0];
						// check if destination has already been reached
						if(dest_map.find(op) == dest_map.end())
						{
							// increase in and out degree counter or write edge
							if( compact )
								{
									out_d++; in_d[op]++;
									L << labels[o]; 
								}
							else
							{
								if(suprema){ ofile << n-b << " " << labels[o] << " " << n-op << "\n"; }
								else{ ofile << b << " " << labels[o] << " " << op << "\n"; }
							}
							// increase edges count
							m++;
							// update set
							dest_map.insert(op);
						}
					}
				}
			}
			// write out degree if needed
			if( compact )
				Out << out_d << "\n";
			// stop when reaching the last state
			if( suprema )
				curr = curr->prev;
			else
				curr = curr->next;
		}
		// close output files
		if( compact )
		{
			// write in degrees
			for(uint_t i=0;i<n;++i)
				In << in_d[i] << "\n";
			// close output streams
			L.close(); In.close(); Out.close(); 
		}
		else
			ofile.close(); 
	}

	/*
		function to store node positios in the partition
	*/
	void store_state_intervals(std::string out, partition& P, uint_t n)
	{
		std::ofstream ofile;
		ofile.open(out); 
		part* curr = P.give_head();
		uint_t s = 0;
		// visit partition P and compute state positions
		while(true)
		{
			for (auto const &i: *curr->nodes)
			{
        		//std::cout << i << " ";
        		if(NFA[i].out.size() > 0)
        		{
        			NFA[i].out[0] = s;
        		}
        		else
        		{
        			NFA[i].out.push_back(s);
        		}
    		}
    		if(curr->next == nullptr){ break; }
    		s++;
    		curr = curr->next;
		}
		// write state intervals
		for(uint_t i=0;i<n;++i)
		{
		  ofile << NFA[i].out[0] << "\t" << NFA[i+n].out[0] << "\n";
		}
		// close output file
		ofile.close();
	}

private:
	// number of nodes in the DFA
	uint_t nodes;
	// number of edges in the DFA
	uint_t edges;
	// source node
	uint_t source;
	// vector containing all edges
	std::vector<node> NFA;
	// vector containing all labels
	std::vector<char> labels;
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