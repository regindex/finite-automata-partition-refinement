#ifndef FINITE_AUTOMATON_HPP_
#define FINITE_AUTOMATON_HPP_

#include "partition.hpp"

// class for general input consistent automata
class Automata_input_consistent{

public:
	Automata_input_consistent():nodes(0),edges(0),source(0){}
	Automata_input_consistent(uint_t nodes_, uint_t source_):nodes(nodes_),edges(0),source(source_)
	{
		labels = std::vector<char>(nodes,-1);
		transitions.resize(nodes);
	}

	void set_edge(uint_t origin, uint_t dest, char label)
	{
		transitions[origin].insert(dest);
		if(labels[dest] == -1)
			labels[dest] = label;
	}

	void print()
	{
		for(uint_t i=0;i<nodes;++i)
			for (const auto& e: transitions[i])
				std::cout << "S" << i+1 << " -> " << "S" << e+1 << " [" << labels[e] << "]" << std::endl;
	}

	uint_t get_n(){ return nodes; }
	char getLabel(uint_t i){ return labels[i]; }
	char getEdge(uint_t i, uint_t j)
	{
		if( transitions[i].find(j) != transitions[i].end() )
			return labels[j];
		else
			return -1;
	}
	char getNull(){ return -1; }
	std::vector<std::pair<uint_t,char>> getEdgeList(uint_t id)
	{
		std::vector<std::pair<uint_t,char>> res;
		for(auto i=transitions[id].begin();i!=transitions[id].end();++i)
			res.push_back(std::make_pair(*i,labels[*i]));

		return res;
	}

	void store_graphML(std::string output_file_name, std::vector<std::vector<uint_t>> chain_dec)
	{
		std::string header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		                     "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n"
		                     "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
		                     "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\n"
		                     "    http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n"
		                     "    <key id=\"chains\" for=\"node\" attr.name=\"chains\" attr.type=\"int\">\n"
		                     "        <default>-1</default>\n"
		                     "    </key>\n"
		                     "    <key id=\"positions\" for=\"node\" attr.name=\"positions\" attr.type=\"int\">\n"
		                     "        <default>-1</default>\n"
		                     "    </key>\n"
		                     "    <key id=\"final\" for=\"node\" attr.name=\"final\" attr.type=\"boolean\">\n"
		                     "        <default>false</default>\n"
		                     "    </key>\n"
		                     "    <key id=\"MN-class\" for=\"node\" attr.name=\"MN-class\" attr.type=\"int\"/>\n"
		                     "    <key id=\"label\" for=\"edge\" attr.name=\"label\" attr.type=\"string\"/>\n"
		                     "    <key id=\"initial_state\" for=\"graph\" attr.name=\"initial_state\" attr.type=\"int\">\n"
		                     "        <default>-1</default>\n"
		                     "    </key>\n"
		                     "    <graph id=\"G\" edgedefault=\"directed\">\n"
		                     "        <data key=\"initial_state\">0</data>\n\n";


		std::string footer = "    </graph>\n"
							 "</graphml>)";

		std::ofstream output_file(output_file_name);
		output_file << header;

		// store automaton's nodes
		for(size_t i=0;i<chain_dec.size();++i)
			for(size_t j=0;j<chain_dec[i].size();++j)
			{
				output_file << "\t\t<node id=\"" << chain_dec[i][j] << "\">\n";
				output_file << "\t\t\t<data key=\"final\">0</data>\n";
				output_file << "\t\t\t<data key=\"chains\">" << i << "</data>\n";
				output_file << "\t\t\t<data key=\"positions\">" << j << "</data>\n";
				output_file << "\t\t</node>\n\n";
			}
		// store automaton's edges
		for(size_t i=0;i<this->nodes;++i)
			for(auto j=this->transitions[i].begin();j!=this->transitions[i].end();++j)
			{
				output_file << "\t\t<edge source=\"" << i << "\" target=\"" << *j << "\">\n";
				output_file << "\t\t\t<data key=\"label\">" << this->labels[*j] << "</data>\n";
				output_file << "\t\t</edge>\n\n";
			}

		output_file << footer;
		output_file.close();

		return;
	}

private:
	// number of nodes in the automaton
	uint_t nodes;
	// number of edges in the automaton
	uint_t edges;
	// source node id
	uint_t source;
	// vector containing all labels
	std::vector<char> labels;
	// vector containing stare transitions
	std::vector<std::unordered_set<uint_t>> transitions;
};

// class for directed bidirectional unlabeled automaton
class NFA_unidirectional_out_unlabelled
{
public:
	/* node of the automaton */
	struct node
	{
		// free allocated memory
		void clear()
		{ 
			for(size_t i=0;i<count.size();++i)
		        { 
		        	if(*count[i] <= 1){ delete count[i]; count[i] = nullptr; }
		        	else{ *count[i] -= 1; }
		        }
		    count.clear(); count.shrink_to_fit();
		    out.clear(); out.shrink_to_fit();
		}
		// out edges
		std::vector<uint_t> out;
		// pointer to partition	
		part* out_part;
		// edge pointers to counts
		std::vector<uint_t*> count;
	};
	// empty constructor
	NFA_unidirectional_out_unlabelled(){}
	// destructor
	~NFA_unidirectional_out_unlabelled(){}
	// constructor 
	NFA_unidirectional_out_unlabelled(uint_t no_nodes, 
									  std::vector<std::tuple<uint_t,uint_t,int>>& edge_list,
									  partition& P)
	{
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
    		int label =  std::get<2>(edge_list[i]);
    		add_edge(std::get<0>(edge_list[i]),std::get<1>(edge_list[i]),label,P);
  		}
  		// delete pointers to initial parts
  		P.delete_spoint();
  		counts.resize(0); counts.shrink_to_fit();
	}
	// initialize the empty NFA
	NFA_unidirectional_out_unlabelled(uint_t nodes_, uint_t source_): nodes(nodes_), source(source_)
	{
		// set edge number
		edges = 0; 
		// initialize DFA
		NFA.resize(nodes);
		// initialize counts
		counts.resize(nodes);
		for(uint_t i=0;i<nodes;++i)
		{
			counts[i] = new uint_t;
			*counts[i] = 0;
		}
	}

	// get number of states in the automaton
	uint_t no_nodes(){ return nodes; }
	// get number of edges in the automaton
	uint_t no_edges(){ return edges; }
	// get a pointer to the ith state
	node* at(uint_t i)
	{
		assert(i < nodes);
		// return pointer to node
		return &NFA[i];
	}
	// print automaton transitions
	void print()
	{
		for(uint_t i=0;i<nodes;++i)
			for (const auto& e: NFA[i].out)
				std::cout << "S" << i << " -> " << "S" << e << " [" << labels[e]-1 << "]" << std::endl;
	}
 	// set automaton source state
	void set_source(uint_t source_){ this->source = source_; }
	// get automaton source state
	uint_t give_source(){ return this->source; }
	// get pointer to ingoing stare labels
	std::vector<char>* give_labels(){ return &this->labels; }
	// initialize labels vector
	void init_label_vector(){ labels = std::vector<char>(nodes,0); }
	// delete temporary counts vector
	void delete_counts()
	{
		delete counts[source];
		counts.clear();
	}
	// delete temporary counts vector for the infsup automaton
	void delete_counts_merge(uint_t n)
	{
		delete counts[source]; delete counts[source+n];
		counts.clear();
	}

	/* function that inserts an empty state in the automaton */
	void add_node()
	{ 
		uint_t curr_size = NFA.size();
		NFA.resize(curr_size+1);
		labels.resize(curr_size+1);
		labels[curr_size] = 0;

		counts.resize(curr_size+1);
		counts[curr_size] = new uint_t;
		*counts[curr_size] = 0;

		this->nodes += 1;
	}
	/* 
		function that inserts a new transition in the automaton,
		and update the partition accordingly
	*/
	void add_edge(uint_t origin, uint_t dest, int label, partition& P)
	{
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
	/* 
		function that adds a dummy stare to compute the automaton,
	  	recognizing suprema strings
	*/
	void add_dummy_state(partition& P)
	{
		// add dummy node in the partition
		P.add_node(nodes, sigma_ascii - 1);
		// add dummy node in the automaton
		NFA.push_back(node());
		// create new count value
		counts.push_back(nullptr);
		counts[nodes] = new uint_t;
		*counts[nodes] = 1;
		// make the dummy state the new source state
		NFA[nodes].out = NFA[source].out;
		NFA[nodes].count = NFA[source].count;
		NFA[nodes].out_part = P.give_part(sigma_ascii - 1);
		// connect the old source to the new one
		NFA[source].out = std::vector<uint_t>{nodes};
		NFA[source].count = std::vector<uint_t*>{counts[nodes]}; 
	}
	/*
		function that assigns a label to an automaton's state and returns true,  
		otherwise, if the transition violates the input consistency property, 
		it returns false
	*/
	bool add_label(uint_t dest, char label)
	{
		if(labels[dest] == 0){ labels[dest] = label; }
		else if(labels[dest] != label)
		{
			//std::cerr << "The input automaton is not input-consistent!" << std::endl;
			return false;
		}

		return true;
	}
	/*
		function that assigns a label to an automaton's state and enforces
		the input concistency property by retaining the smallest or largest
		label of each automaton's stare
	*/
	int assign_remove_label(uint_t dest, char label, bool suprema)
	{
		int flag = 0; // by default we do not change the label assigned
					  // to a node 
		if(labels[dest] == 0)
			labels[dest] = label;
		else if(labels[dest] != label)
		{
			flag = -1; // if the new label is different than the current one
					   // we remove one of the two --> by default we remove
					   // the new one.
			if(suprema)
			{
				if(label > labels[dest])
				{
					flag = labels[dest]; // remove the previous label
					labels[dest] = label;
				}
			}
			else
			{
				if(label < labels[dest])
				{
					flag = labels[dest]; // remove the previous label
					labels[dest] = label;
				}
			}
		}
		return flag;
	}

	/*
		function that computes the mapping between automaton's states and
		partition's parts for Wheeler pseudo-forests
	*/
	uint_t compute_mapping_pforest(partition& P)
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
				// add one count if the node has no edges
				if( NFA[e].count.size() == 0 ){ NFA[e].count.push_back(new uint_t(0)); }
				*(NFA[e].count[0]) = cnt;
			}
			// increase counter
			cnt++;
			// stop when reaching the last state
			curr = curr->next;
		}

		return cnt;
	}
	/*
		function that outputs a Wheeler pseudo-forest in either dot format
		or compact format
	*/
	void to_output_Wheeler_pforest(std::string out, partition& P, bool compact = true)
	{
		// initialize and open output files
		std::ofstream ofile, L, In, Out;
		// initialize needed variables
		uint_t out_d = 0, n = 0, m = 0;
		std::vector<uint_t> in_d, in_l; 

		if( compact )
		{
			L.open(out+".L"); In.open(out+".in"); Out.open(out+".out"); 
		}
		else
			ofile.open(out); 

		// compute mapping state -> part
		n = compute_mapping_pforest(P);
		// init indegree vector or write no. states to file
		if( compact )
		{
			in_d = std::vector<uint_t>(n,0);
			in_l = std::vector<uint_t>(sigma_ascii,0);
		}
		else
			ofile << "strict digraph {\n";
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
								L << char(labels[o] - 1); 
							}
						else
						{
							ofile << "	S" << b+1 << " -> S" << op+1 << " [ label = " << uint_t(labels[o] - 1) << " ];\n";
							//std::cout << "	S" << b+1 << " -> S" << op+1 << " [ label = " << uint_t(labels[o] - 1) << " ];\n";
						}
						// increase edges count
						m++;
						// update set
						dest_map.insert(op);
					}
				}
			}
			// write out degree if needed
			if( compact )
				Out << std::string(out_d,'0') << '1';
			// stop when reaching the last state
			curr = curr->next;
		}
		// close output files
		if( compact )
		{
			// write in degrees
			for(uint_t i=0;i<n;++i)
				In << std::string(in_d[i],'0') << '1';
			// close streams
			L.close(); In.close(); Out.close(); 
		}
		else
		{
			ofile << "}";
			ofile.close(); 
		}
	}
	/*
		function that computes the mapping between automaton's states and
		partition's parts for arbitrary automata
	*/
	uint_t compute_mapping(partition& P, std::vector<uint_t>& mapping)
	{
		// get head of the partition
		part* curr = P.give_head();
		// set node counter
		uint_t cnt = 0;
		// iterate over all parts
		while(curr != nullptr)
		{	
			// interate over nodes in each part and assign the new node ids
			for (const auto& e: *curr->nodes)
				mapping[e] = cnt;
			// increase counter
			cnt++;
			// stop when reaching the last state
			curr = curr->next;
		}

		return cnt;
	}
	/*
		function that outputs to file the resulting automaton computed
		by remapping the original states using the sorted partition
	*/
	void to_output_sorted(std::string out, partition& P, bool compact = true)
	{
		// initialize and open output files
		std::ofstream ofile, L, In, Out;
		// initialize needed variables
		uint_t out_d = 0, n = 0, m = 0, p = 0;
		std::vector<uint_t> in_d, in_l; 
		// mapping state to part
		std::vector<uint_t> mapping(NFA.size(),0);
		// vector storing 
		if( compact )
		{
			L.open(out+".L"); In.open(out+".in"); Out.open(out+".out"); 
		}
		else
			ofile.open(out); 

		// compute mapping state -> part
		n = compute_mapping(P,mapping);

		// init indegree vector or write no. states to file
		if( compact )
		{
			in_d = std::vector<uint_t>(n,0);
			in_l = std::vector<uint_t>(sigma_ascii,0);
		}
		else
			ofile << "strict digraph {\n";
		// set current part to head of partition
		part* curr = P.give_head();
		// iterate over partition and output automaton
		while(curr != nullptr)
		{	
			// set containing reached states
			std::unordered_set<uint_t> dest_map;
			// set outdegree to zero
			out_d = 0;
			// increase partition counter
			p++;
			// iterate over nodes in part
			for (const auto& e: *curr->nodes)
			{
				// iterate over outgoing edges
				for(uint_t i=0;i<NFA[e].out.size();++i)
				{
					// compute reached state and label
					uint_t o = NFA[e].out[i];				
					uint_t op = mapping[o];
					// check if destination has already been reached
					if(dest_map.find(op) == dest_map.end())
					{
						// increase in and out degree counter or write edge
						if( compact )
							{
								out_d++; in_d[op]++; in_l[labels[o]]++;
								L << char(labels[o] - 1); 
							}
						else
						{
							ofile << "	S" << p << " -> S" << op+1 << " [ label = " << uint_t(labels[o] - 1) << " ];\n";
							// std::cout << "	S" << p << " -> S" << op+1 << " [ label = " << uint_t(labels[o] - 1) << " ];\n";
						}
						// increase edges count
						m++;
						// update set
						dest_map.insert(op);
					}
				}
			}
			// write out degree if needed
			if( compact )
				Out << std::string(out_d,'0') << '1';
			// stop when reaching the last state
			curr = curr->next;
		}
		// close output files
		if( compact )
		{
			// write in degrees
			for(uint_t i=0;i<n;++i)
				In << std::string(in_d[i],'0') << '1';
			// close streams
			L.close(); In.close(); Out.close(); 
		}
		else
		{
			ofile << "}";
			ofile.close(); 
		}
	}
	/*
		function that returns the resulting automaton computed
		by remapping the original states using the sorted partition
	*/
	void get_sorted_automaton(Automata_input_consistent& sortedAutomaton, partition& P)
	{
		// initialize needed variables
		uint_t out_d = 0, n = 0, m = 0, p = 0;
		std::vector<uint_t> in_d, in_l; 
		// mapping state to part
		std::vector<uint_t> mapping(NFA.size(),0);

		// compute mapping state -> part
		n = compute_mapping(P,mapping);
		sortedAutomaton = Automata_input_consistent(n,0);

		// set current part to head of partition
		part* curr = P.give_head();
		// iterate over partition and output automaton
		while(curr != nullptr)
		{	
			// set containing reached states
			std::unordered_set<uint_t> dest_map;
			// set outdegree to zero
			out_d = 0;
			// increase partition counter
			p++;
			// iterate over nodes in part
			for (const auto& e: *curr->nodes)
			{
				// iterate over outgoing edges
				for(uint_t i=0;i<NFA[e].out.size();++i)
				{
					// compute reached state and label
					uint_t o = NFA[e].out[i];				
					uint_t op = mapping[o];
					// check if destination has already been reached
					if(dest_map.find(op) == dest_map.end())
					{
						//std::cout << "	S" << p << " -> S" << op+1 << " [ label = " << uint_t(labels[o] - 1) << " ];\n";
						sortedAutomaton.set_edge(p-1,op,uint_t(labels[o] - 1));
						// increase edges count
						m++;
						// update set
						dest_map.insert(op);
					}
				}
			}
			// stop when reaching the last state
			curr = curr->next;
		}
	}
	/*
		function that outputs to file the resulted automaton computed
		by running the pruning algorithm
	*/
	void to_output_pruned(std::string out)
	{
		// fix automaton if we added the dummy state
		if( nodes < NFA.size() )
			NFA[source] = NFA[nodes];

		// open output file 
		std::ofstream ofile;
		ofile.open(out); 
		// write first line
		ofile << "strict digraph {\n";
		// scan the graph
		for(uint_t i=0;i<nodes;++i)
		{
			for(uint_t j=0;j<NFA[i].out.size();++j)
			{
				if( *NFA[i].count[j] > 0 )
				{
					// ofile << i << " " << labels[NFA[i].out[j]] << " " << NFA[i].out[j] << "\n";
					ofile << "	S" << i+1 << " -> S" << NFA[i].out[j]+1 << " [ label = " << uint_t(labels[NFA[i].out[j]] - 1) << " ];\n";
				}
			}
		}
		// write dot last line
		ofile << "}";
		// close output file
		ofile.close();
	}
	/* function to store node positions in the partition */
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

	/* free allocated memory */
	void clear()
	{
		for(size_t i=0;i<NFA.size();++i)
			NFA[i].clear();
		NFA.clear(); NFA.shrink_to_fit();
		labels.clear(); labels.shrink_to_fit();
		if(counts.size() > 0)
			delete_counts();
	}

private:
	// number of nodes in the automaton
	uint_t nodes;
	// number of edges in the automaton
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

#endif /* FINITE_AUTOMATON */