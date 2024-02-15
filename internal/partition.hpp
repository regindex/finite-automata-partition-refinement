#ifndef PARTITION_HPP_
#define PARTITION_HPP_

#include "definitions.hpp"

/* 
	node representing a part in the partition 
*/
struct part
{
	// Pointer to next part
    part* next;	
    // Pointer to previous part
    part* prev;	
    // set of nodes in the part
    std::unordered_set<uint_t>* nodes;
};

/* 
	node of the compound block list C
*/
struct node_compound
{
	// Pointer to next part
    part* first;	
    // set of nodes in the part
    part* last;
};

// Doubly linked list
class partition {

public:
	// empty constructor
	partition(){}
	// constructor 
	partition(int sigma, uint_t source, std::vector<std::tuple<uint_t,uint_t,int>>& edge_list){

		// label pointer vector initialization
		spoint.resize(sigma);
		// initialize head
		head = new part();
		head->nodes = new std::unordered_set<uint_t>;
		// insert source
		head->nodes->insert(source);
		head->prev = nullptr;
		spoint[0] = head;
		// initialize other initial parts
		part * prev = head;
		for(int i=1;i<sigma;++i)
		{
			part* node = new part();
			node->nodes = new std::unordered_set<uint_t>;
			prev->next = node;
			spoint[i] = node;
			node->prev = prev;
			// update previous node
			prev = node;
		}
		// set last node pointer
		prev->next = nullptr;
		// iterate over edge list
	  	for(uint_t i=0;i<edge_list.size();++i)
	  	{
	    	int label = std::get<2>(edge_list[i]);
	    	spoint[label]->nodes->insert(std::get<1>(edge_list[i]));
	  	}
	  	// compute first S in C and remove empty partitions
	  	set_first_C_block();
	}
	// constructor 
	partition(int sigma, uint_t source){

		// label pointer vector initialization
		spoint.resize(sigma);
		// initialize head
		head = new part();
		head->nodes = new std::unordered_set<uint_t>;
		// insert source
		head->nodes->insert(source);
		head->prev = nullptr;
		spoint[0] = head;
		// initialize other initial parts
		part * prev = head;
		for(int i=1;i<sigma;++i)
		{
			part* node = new part();
			node->nodes = new std::unordered_set<uint_t>;
			prev->next = node;
			spoint[i] = node;
			node->prev = prev;
			prev = node;
		}
		// set last node pointer
		prev->next = nullptr;
	}

	void add_source(uint_t source){

		// add new source to head node
		head->nodes->insert(source);
	}

	void set_first_C_block(){

		// instantiate first compound block
		std::pair<part*,part*>* firstC = new std::pair<part*,part*>;
		firstC->first = head;
		part* curr = head->next;
		part* prev = head;
		// mark beginning of the compound block
		firstP.insert({head,firstC});
		while(true)
		{
			// check if the current part is the last one
			if(curr->next == nullptr)
			{ 
				if(curr->nodes->size() > 0)
				{
					prev->next = curr; 
					curr->prev = prev;
					firstC->second = curr;
				}
				else
				{
					prev->next = nullptr;
					firstC->second = prev;
				}
				break;
			}
			// check if the part is empty
			if(curr->nodes->size() > 0)
			{
				prev->next = curr;
				curr->prev = prev;
				prev = curr;
				curr = curr->next;
			}
			else
			{
				part* temp = curr->next;
				delete curr;
				curr = temp;
			}
		}
		// mark end of the compound block
		lastP.insert({firstC->second,firstC});
		// insert the first compound block in C
		C.push(firstC);
	}

	/*
		return S plus a bool to tell which is B
	*/
	std::pair<bool,std::pair<part*,part*>*> get_B(){

		bool first = true;
		bool found = false;

		std::pair<part*,part*>* S;
		while( !C.empty() )
		{
			S = C.top();
			C.pop();
			if(S->first != S->second)
			{
				found = true; break;
			}
		}
		// handle case where no splitter is found
		if( !found )
		{
			S = new std::pair<part*,part*>;
			return std::make_pair(first,S);
		}

		uint_t size_first, size_last;
		// check splitter sizes
		size_first = S->first->nodes->size();
		size_last = S->second->nodes->size();

		if(size_first > size_last){ first = false; }

		return std::make_pair(first,S);
	}


	/*
		return a pointer to one of the initial parts
	*/
	part* give_part(int label)
	{
		// return pointer to part
		return spoint[label];
	}

	/*
		return a pointer to the list head
	*/
	part* give_head()
	{
		// return pointer to head
		return head;
	}

	/*
		add a node in one of the initial part
	*/
	void add_node(uint_t dest, int label)
	{
		// add node in a part
		spoint[label]->nodes->insert(dest);
	}

	/*
		update the table containing the pointers at the first and last parts of the
		compound blocks
	*/
	void update_first_last_part(part* oldP, part* newP, std::pair<part*,part*>* Cb,
								std::pair<part*,part*>* B, bool first)
	{
		// update B
		firstP[oldP] = B;
		lastP[oldP] = B;

		if( first )
		{
			firstP.insert({newP, Cb});
		}
		else
		{
			lastP.insert({newP, Cb});
		}
	}

	/*
		remove the pointers at the first and last parts of the compound blocks
	*/
	void remove_first_last_part(part* oldP)
	{
		// update B
		firstP.erase(oldP);
		lastP.erase(oldP);
	}

	/*
		function to print P
	*/
	void print_partition()
	{
		part* curr = head;
		std::cout << "|| ";
		while(true)
		{
			for (auto const &i: *curr->nodes)
			{
        		std::cout << i << " ";
    		}
    		std::cout << "|| ";
    		if(curr->next == nullptr){ break; }
    		curr = curr->next;
		}
		std::cout << '\n';
	}

	void print_partition_size()
	{
		part* curr = head;
		std::cout << "|| ";
		while(true)
		{
			std::cout << curr->nodes->size();
    		std::cout << " || ";
    		if(curr->next == nullptr){ break; }
    		curr = curr->next;
		}
		std::cout << '\n';
	}

	/*
		function to print P to file
	*/
	void to_file(std::string out)
	{
		FILE *out_file;
		// open output files
	    if((out_file = fopen(out.c_str(), "w+")) == nullptr){
	        std::cerr << "open() file " + out + " failed!" << std::endl;
	        exit(-1);
	    }
	    uint_t sep = U_MAX;
		part* curr = head;
		while(true)
		{
			for (auto const &i: *curr->nodes)
			{
        		if(fwrite(&i,sizeof(uint_t),1,out_file) != 1)
        		{
        			std::cerr << "Out file write error... exiting!" << std::endl;
                	exit(-1);	
        		}
    		}
    		// write separator
    		if(fwrite(&sep,sizeof(uint_t),1,out_file) != 1)
    		{
    			std::cerr << "Sep file write error... exiting!" << std::endl;
            	exit(-1);	
    		}
    		// check next node
    		if(curr->next == nullptr){ break; }
    		curr = curr->next;
		}
		// close output file
		fclose(out_file);
	}

	/*
		tell if a part is the beginning of a compound block
	*/
	bool is_first(part* p)
	{
		return firstP.find(p) != firstP.end();
	}

	/*
		tell if a part is the end of a compound block
	*/
	bool is_last(part* p)
	{
		return lastP.find(p) != lastP.end();
	}

	void print_last()
	{
		for (auto entry : lastP)
	    {
	      std::cout << entry.first << " : ";
	    }
	}

	/*
		update the beginning of a compound block
	*/
	void update_first(part* p1, part* p2)
	{
		std::pair<part*,part*>* c = firstP.at(p1);
		firstP.insert({p2,c});
		c->first = p2;
		firstP.erase(p1);
	}

	/*
		update the beginning of a compound block and return the pointer
	*/
	std::pair<part*,part*>* update_return_first(part* p1, part* p2)
	{
		std::pair<part*,part*>* c = firstP.at(p1);
		firstP.insert({p2,c});
		c->first = p2;
		firstP.erase(p1);

		return c;
	}

	/*
		update the end of a compound block 
	*/
	void update_last(part* p1, part* p2)
	{
		std::pair<part*,part*>* c = lastP.at(p1);
		lastP.insert({p2,c});
		c->second = p2;
		lastP.erase(p1);
	}

	/*
		update the end of a compound block and return the pointer
	*/
	std::pair<part*,part*>* update_return_last(part* p1, part* p2)
	{
		std::pair<part*,part*>* c = lastP.at(p1);
		lastP.insert({p2,c});
		c->second = p2;
		lastP.erase(p1);

		return c;
	}

	/*
		remove a part in the partition
	*/
	void remove_part(part* current)
	{
		current->prev->next = current->next;
		if(current->next != nullptr)
			current->next->prev = current->prev;
		delete current;
	}

	/*
		add a new part in the partition
	*/
	void insert(part* current, part* new_part)
	{
		new_part->next = current->next;
		new_part->prev = current;
		current->next = new_part;
		if( new_part->next != nullptr )
			new_part->next->prev = new_part;
	}

	/*
		insert a compound block in C
	*/
	void insert_C(std::pair<part*,part*>* c)
	{
		C.push(c);
	}

	/*
		return number of compound blocks
	*/
	uint_t C_size(){ return C.size(); }

	/*
		todo add description
	*/
	void delete_spoint()
	{
		// delete pointers to initial parts
		spoint.resize(0);
		spoint.shrink_to_fit();
	}

	/*
		todo add description
	*/
	void add_part(part* current, part* new_part)
	{
		part* temp = current->next;
		current->next = new_part;
		new_part->next = temp;
	}

private:
	// number of edges in the NFA
	part* head;
	// pointers to initial parts
	std::vector<part*> spoint; 
	// stack containing compound blocks
	std::stack<std::pair<part*,part*>*> C; 
	// first parts of compound blocks 
	std::unordered_map<part*,std::pair<part*,part*>*> firstP;
	// last parts of compound blocks 
	std::unordered_map<part*,std::pair<part*,part*>*> lastP;
};


#endif