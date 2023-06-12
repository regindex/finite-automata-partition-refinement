/*
	Code implementing partition refinement algorithm.
*/

#include "part_ref.h"

/**
 * @brief Sort a Wheeler NFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA input consistent NFA to sort
 */
void PART_REF_NFA(partition& P, graph& NFA) {

	// iterate until C is empty
	while(true)
  	{
  		// get splitter
	    std::pair<bool,std::pair<part*,part*>*> S = P.get_B();
	    std::pair<part*,part*>* B = new std::pair<part*,part*>;
	    B->first = B->second = nullptr;

	    // check termination condition
	    if( S.second->first == nullptr ){ break; } 

	    #ifdef VERBOSE
		    {
			    std::cout << "------------Print S---\n";
			    for (const auto& elem: *S.second->first->nodes) {
					std::cout << elem << " ";
				}
				std::cout << "\n";
				for (const auto& elem: *S.second->second->nodes) {
					std::cout << elem << " ";
				}
				std::cout << "\n";
			}
		#endif

	    // create B and S without B sets
	    if( S.first )
	    {
	     	B->first = B->second = S.second->first;
	    	P.update_first_last_part(S.second->first,S.second->first->next,S.second,B,true);
	    	S.second->first = S.second->first->next;
	    }
	    else
	    {
	      	B->first = B->second = S.second->second;
	    	P.update_first_last_part(S.second->second,S.second->second->prev,S.second,B,false);
	    	S.second->second = S.second->second->prev;
	    }

	    // check if we can push S_w_B back in C
	    if( S.second->first != S.second->second ){ P.insert_C(S.second); }

	    #ifdef VERBOSE
	    {
		    std::cout << "###################\n";
		    P.print_partition();
		    std::cout << "B = { ";
		    for (auto const &i: *B->first->nodes)
		    {
		      std::cout << i << " ";
		    }
		    std::cout << "}\n";
		    std::cout << "---------\n";
		}
		#endif

	    std::unordered_set<uint_t> Bsp = *B->first->nodes;
	    std::unordered_map<part*,std::pair<std::unordered_set<uint_t>*,std::unordered_set<uint_t>*>> D1;
	    std::unordered_map<uint_t,uint_t*> CU;

	    // iterate over all nodes in B
	    for (const auto& i: Bsp)
	    {
	    	// number of outgoing edges
	    	uint_t out_size = NFA.at(i)->out.size();

	    	#ifdef VERBOSE
	    	{
	    		std::cout << "b : " <<  i << " -> ";
	    	}
	    	#endif

	    	for(uint_t j=0; j<out_size; ++j)
		    {
		    	#ifdef VERBOSE
		    	{
		    		std::cout << NFA.at(i)->out[j] << " ";
		    	}
		    	#endif

		        part* key =  NFA.at(NFA.at(i)->out[j])->out_part;
		        // in case we are pointing to a part with 1 element, skip it
		        if( key->nodes->size() == 1 && D1.find(key) == D1.end() ){ continue; }
		        // update D1
		        if ( D1.find(key) == D1.end() )
		        {
		          std::unordered_set<uint_t>* new_D11 = new std::unordered_set<uint_t>;
		          std::unordered_set<uint_t>* new_D12 = new std::unordered_set<uint_t>;
		          new_D11->insert(NFA.at(i)->out[j]);
		          D1.insert({key,std::make_pair(new_D11,new_D12)});
		        }
		        else
		        {
		          D1.at(key).first->insert(NFA.at(i)->out[j]);
		        }
		        // update D2
		        key->nodes->erase(NFA.at(i)->out[j]);

		        // create D11 and D12
		        *NFA.at(i)->count[j] -= 1;
		        if( *NFA.at(i)->count[j] == 0 )
		        {
		          D1.at(key).second->insert(NFA.at(i)->out[j]);
		          D1.at(key).first->erase(NFA.at(i)->out[j]);
		          // delete the count when it reaches zero
		          delete NFA.at(i)->count[j];
		        }

		        // check if we need to create a new edge count
		        if( CU.find(NFA.at(i)->out[j]) == CU.end() )
		        {
		          NFA.at(i)->count[j] = new uint_t;
		          *NFA.at(i)->count[j] = 1;
		          CU.insert({NFA.at(i)->out[j],NFA.at(i)->count[j]});
		        }
		        else
		        {
		          NFA.at(i)->count[j] = CU.at(NFA.at(i)->out[j]);
		          *NFA.at(i)->count[j] += 1; 
		        }
	      	} 
	      	#ifdef VERBOSE
	      	{
	      		std::cout << "\n";
	      	}
	      	#endif
	    }

	    #ifdef VERBOSE
	    {
	    	// print D1
		    std::cout << "D11\n";
		    for (auto entry : D1)
		    {
		      std::cout << entry.first << " : ";
		      for (auto const &i: *entry.second.first)
		      {
		        std::cout << i << " ";
		      }
		      std::cout << "\n";
		    }
		    // print D2
		    std::cout << "D12\n";
		    for (auto entry : D1)
		    {
		      std::cout << entry.first << " : ";
		      for (auto const &i: *entry.second.second)
		      {
		        std::cout << i << " ";
		      }
		      std::cout << "\n";
		    }
		    P.print_partition();
		    std::cout << "---------\n";
		}
		#endif
	    //####### update the partition P ###########//
	    for (auto entry : D1)
	    {
	      	// create D12 part
	      	part* part_D12 = nullptr;
	     	if( entry.second.second->size() > 0 )
	      	{
	        	part_D12 = new part();
	        	part_D12->nodes = entry.second.second;
	        	for (auto const &i: *part_D12->nodes)
	        	{
	          		NFA.at(i)->out_part = part_D12;
	        	}
	      	}
	      	// create D11 part
	      	part* part_D11 = nullptr;
	      	if( entry.second.first->size() > 0 )
	      	{
	        	part_D11 = new part();
	        	part_D11->nodes = entry.second.first;
	        	// add_part(part_D12,part_D11);
	        	for (auto const &i: *part_D11->nodes)
	        	{ 
	          	NFA.at(i)->out_part = part_D11;
	        	}
	      	}
	      	// ###### insert new parts ######
	      	part* first = nullptr; part* last = nullptr;
	      	int no_new_parts = 1;
	      	// if B is the first part in the compound block -> D12 D11 D2
	      	if( S.first )
	      	{
	      		// create new D12 part
	        	if(part_D12 != nullptr)
	        	{
	          		P.insert(entry.first->prev,part_D12);
	          		first = part_D12;
	          		no_new_parts++;
	        	}
	        	// create new D11 part
	        	if(part_D11 != nullptr)
	        	{
	          		P.insert(entry.first->prev,part_D11);
	          		if(first == nullptr) { first = part_D11; }
	          		no_new_parts++;
	        	}
	        	// remove D2 part if it is empty
	        	bool D2_rem = false;
	        	if(entry.first->nodes->size() == 0)
	        	{ 
		        	no_new_parts--;
		        	last = entry.first->prev;
		        	P.remove_part(entry.first);
		        	D2_rem = true;
	        	}
	        	// check if entry.first has been made compound by this split
	        	bool is_last = P.is_last(entry.first);
	        	bool is_first = P.is_first(entry.first);

	        	if( first != nullptr && is_first )
	        	{
	        		//P.update_first(entry.first,first);
	        		//is_last = P.is_last(entry.first);
	        		if( no_new_parts > 1 && is_last )
	        		{
	        			// if true entry.first was not compound before this split
	        			P.insert_C( P.update_return_first(entry.first,first) );
	        		} // else if it was already a compound
	        		else{ P.update_first(entry.first,first); }
	        	}
	        	// if we removed D2 update the pointer to last element of compound
	        	if( D2_rem && is_last ) P.update_last(entry.first,last);
	      	}
	      	else // if B is the last part in the compound block -> D2 D11 D12
	      	{
		      	// create new D12 part
		        if(part_D12 != nullptr)
		        { 
		          P.insert(entry.first,part_D12);
		          last = part_D12;
		          no_new_parts++;
		        }
		        // create new D11 part
		        if(part_D11 != nullptr)
		        {
		          P.insert(entry.first,part_D11);
		          if(last == nullptr) { last = part_D11; }
		          no_new_parts++;
		        }
		       	// remove D2 part if it is empty
	        	bool D2_rem = false;
	        	if(entry.first->nodes->size() == 0)
	        	{ 
		        	no_new_parts--;
		        	first = entry.first->next;
		        	P.remove_part(entry.first);
		        	D2_rem = true;
	        	}
	        	// check if entry.first has been made compound by this split
	        	bool is_last = P.is_last(entry.first);
	        	bool is_first = P.is_first(entry.first);
	        	if( last != nullptr && is_last )
	        	{
	        		//is_first = P.is_first(entry.first);
	        		if( no_new_parts > 1 && is_first )
	        		{
	        			// if true entry.first was not compound before this split
	        			P.insert_C( P.update_return_last(entry.first,last) );
	        		} // else if it was already a compound
	        		else{ P.update_last(entry.first,last); }
	        	}
	        	// if we removed D2 update the pointer to last element of compound
	        	if( D2_rem && is_first ){ P.update_first(entry.first,first); }
	   	 	}
	    }
	    // insert S_w_B in C
	    if( S.second->first == S.second->second )
	    {
	    	#ifdef VERBOSE
	    	{
	    		std::cout << "S_w_B is not a compound block\n";
	    	}
	    	#endif
	    	if( S.second->first->nodes->size() == 1 )
	     	{
	     		// S_w_B is no longer a compound block
	     		// delete the pointers from the table
		    	#ifdef VERBOSE
		    	{
		    		std::cout << "delete S_w_B\n";
		    	}
		    	#endif
	      		P.remove_first_last_part(S.second->first);
	      		delete S.second;
	      	}
	    }
	    // insert B in C
	    if( B->first == B->second )
	    {
	    	#ifdef VERBOSE
	    	{
	    		std::cout << "B is not a compound block\n";
	    	}
	    	#endif
	    	if( B->first->nodes->size() == 1 )
	    	{
	     		// S_w_B is no longer a compound block
	     		// delete the pointers from the table
	     		#ifdef VERBOSE
		    	{
		    		std::cout << "delete B\n";
		    	}
		    	#endif
		    	P.remove_first_last_part(B->first);
		    	delete B;
	    	}
	    }
 	}
}

/**
 * @brief Sort an input consistent DFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param DFA input consistent DFA to sort
 */
void PART_REF_DFA(partition& P, graph& DFA) {

	#ifdef VERBOSE
	{
		std::cout << "########FIRST PARTITION###########\n";
		P.print_partition();
		std::cout << "---------\n";
	}
	#endif

	// iterate until C is empty
	while(true)
  	{
  		//std::cout << "i:" << i; 
  		//i++;
  		//std::cout << "print new B\n";
  		// get splitter B and S_w_B
	    std::pair<bool,std::pair<part*,part*>*> S = P.get_B();
	    std::pair<part*,part*>* B = new std::pair<part*,part*>;
	    B->first = B->second = nullptr;

	    // check termination condition
	    if( S.second->first == nullptr ){ break; } 

	    #ifdef VERBOSE
	    {
		    std::cout << "#################Print S##########\n";
		    for (const auto& elem: *S.second->first->nodes) {
				std::cout << elem << " ";
			}
			std::cout << S.second->first << " " << S.second->first->prev << " " << S.second->first->next;
			std::cout << "\n";
			for (const auto& elem: *S.second->first->next->nodes) {
				std::cout << elem << " ";
			}
			std::cout << "\n";
			for (const auto& elem: *S.second->second->nodes) {
				std::cout << elem << " ";
			}
			std::cout << S.second->second << " " << S.second->second->prev << " " << S.second->second->next;
			std::cout << "\n";
			for (const auto& elem: *S.second->second->prev->nodes) {
				std::cout << elem << " ";
			}
			std::cout << "\n";
		}
		#endif

	    // create B and S without B sets
	    if( S.first )
	    {
	     	B->first = B->second = S.second->first;
	    	P.update_first_last_part(S.second->first,S.second->first->next,S.second,B,true);
	    	S.second->first = S.second->first->next;
	    }
	    else
	    {
	      	B->first = B->second = S.second->second;
	    	P.update_first_last_part(S.second->second,S.second->second->prev,S.second,B,false);
	    	S.second->second = S.second->second->prev;
	    }

	    // check if we can push S_w_B back in C
	    if( S.second->first != S.second->second ){ P.insert_C(S.second); }

	    #ifdef VERBOSE
	    {
		    std::cout << "#################Print B##########\n";
		    P.print_partition();
		    std::cout << "B = { ";
		    for (auto const &i: *B->first->nodes)
		    {
		      std::cout << i << " ";
		    }
		    std::cout << "}\n";
		    std::cout << "---------\n";
		}
		#endif

	    std::unordered_set<uint_t> Bsp = *B->first->nodes;
	    std::unordered_map<part*,std::pair<std::unordered_set<uint_t>*,std::unordered_set<uint_t>*>> D1;
	    std::unordered_map<uint_t,std::pair<uint_t*,uint_t*>> CU;

	    // iterate over all nodes in B in |B| time
	    for (const auto& i: Bsp)
	    {
	    	uint_t out_size = DFA.at(i)->out.size();

	    	#ifdef VERBOSE
	    	{
	      		std::cout << "b : " <<  i << " -> ";
	      	}
	      	#endif 

	    	for(uint_t j=0; j<out_size; ++j)
		    {
		    	if( *DFA.at(i)->count[j] == 0 ){ continue; }

		    	#ifdef VERBOSE
		    	{
		       		std::cout << DFA.at(i)->out[j] << " ";
		       	}
		       	#endif

		        // update count
		        *DFA.at(i)->count[j] -= 1;
		        // update counts update table
	        	if( CU.find(DFA.at(i)->out[j]) == CU.end() )
	        	{
	        		uint_t* new_count = new uint_t;
	        		*new_count = 1;
	        		CU.insert({DFA.at(i)->out[j],std::make_pair(new_count,DFA.at(i)->count[j])});
	          		DFA.at(i)->count[j] = new_count;
	        	}
		        else
		        {
		          DFA.at(i)->count[j] = CU.at(DFA.at(i)->out[j]).first;
		          *DFA.at(i)->count[j] += 1; 
		        }
	      	} // for each element in B
	      	#ifdef VERBOSE
	        std::cout << "\n";
	        #endif
	    } // for each block in B

	    /*if( v )*/
	    #ifdef VERBOSE
	    {
	    	std::cout << "Count update table:\n";
		}
	    #endif
	    for (auto entry : CU)
	    {
	    	#ifdef VERBOSE
	    	{
		      	std::cout << "entry = " << entry.first << " : ";
		      	std::cout << *entry.second.first << " - ";
		      	std::cout << *entry.second.second << "\n";
	      	}
	      	#endif
	    	// its D11
	    	if( *entry.second.second > 0)
	    	{
	    		if( S.first )
	    		{
	    			*entry.second.second = 0;

	    			part* key =  DFA.at(entry.first)->out_part;
	    			if( key->nodes->size() == 1 && D1.find(key) == D1.end() ){ continue; }

	    			if ( D1.find(key) == D1.end() )
			        {
			          std::unordered_set<uint_t>* new_D11 = new std::unordered_set<uint_t>;
			          std::unordered_set<uint_t>* new_D12 = new std::unordered_set<uint_t>;
			          new_D11->insert(entry.first);
			          D1.insert({key,std::make_pair(new_D11,new_D12)});
			        }
			        else
			        {
			          D1.at(key).first->insert(entry.first);
			        }

			        key->nodes->erase(entry.first);
	    		}
	    		else 
	    		{
	    			*entry.second.first = 0;
	    		}
	    	}
	    	else // its D12
	    	{
	    		assert( *entry.second.second == 0 );

	    		part* key =  DFA.at(entry.first)->out_part;
	    		if( key->nodes->size() == 1 && D1.find(key) == D1.end() ){ continue; }

    			if ( D1.find(key) == D1.end() )
		        {
		          std::unordered_set<uint_t>* new_D11 = new std::unordered_set<uint_t>;
		          std::unordered_set<uint_t>* new_D12 = new std::unordered_set<uint_t>;
		          new_D12->insert(entry.first);
		          D1.insert({key,std::make_pair(new_D11,new_D12)});
		        }
		        else
		        {
		          D1.at(key).second->insert(entry.first);
		        }

		        key->nodes->erase(entry.first);
	    	}
	    }

	    // print D1
	    #ifdef VERBOSE
	    {
		    std::cout << "D11\n";
		    for (auto entry : D1)
		    {
		      std::cout << entry.first << " : ";
		      for (auto const &i: *entry.second.first)
		      {
		        std::cout << i << " ";
		      }
		      std::cout << "\n";
		    }
		    // print D2
		    std::cout << "D12\n";
		    for (auto entry : D1)
		    {
		      std::cout << entry.first << " : ";
		      for (auto const &i: *entry.second.second)
		      {
		        std::cout << i << " ";
		      }
		      std::cout << "\n";
		    }
		    
		    P.print_partition();
		    std::cout << "---------\n";
		}
	    #endif
		
		// exit(1);
	    // update the partition
	    for (auto entry : D1)
	    {
	      	// create D12 part
	      	part* part_D12 = nullptr;
	     	if( entry.second.second->size() > 0 )
	      	{
	      		//std::cout << "ENTRATO\n";
	        	part_D12 = new part();
	        	part_D12->nodes = entry.second.second;
	        	for (auto const &i: *part_D12->nodes)
	        	{
	          		DFA.at(i)->out_part = part_D12;
	        	}
	      	}
	      	// create D11 part
	      	part* part_D11 = nullptr;
	      	if( entry.second.first->size() > 0 )
	      	{
	        	part_D11 = new part();
	        	part_D11->nodes = entry.second.first;
	        	// add_part(part_D12,part_D11);
	        	for (auto const &i: *part_D11->nodes)
	        	{ 
	          	DFA.at(i)->out_part = part_D11;
	        	}
	      	}
	      	// ###### insert new parts ######
	      	part* first = nullptr; part* last = nullptr;
	      	int no_new_parts = 1;
	      	if( S.first ) // D12 D11 D2
	      	{
	      		//std::cout << "un first\n";
	      		// create new D12 part
	        	if(part_D12 != nullptr)
	        	{
	        		////std::cout << "prev: " << entry.first->prev << "\n";
	        		////std::cout << "B: " << B->first << "\n";
	          		P.insert(entry.first->prev,part_D12);
	          		first = part_D12;
	          		no_new_parts++;
	        	}
	        	// create new D11 part
	        	if(part_D11 != nullptr)
	        	{
	          		P.insert(entry.first->prev,part_D11);
	          		if(first == nullptr) { first = part_D11; }
	          		no_new_parts++;
	        	}
	        	// remove D2 part if it is empty
	        	bool D2_rem = false;
	        	if(entry.first->nodes->size() == 0)
	        	{ 
	        		assert(no_new_parts > 1);
		        	no_new_parts--;
		        	last = entry.first->prev;
		        	assert(last == part_D11 || last == part_D12);
		        	P.remove_part(entry.first);
		        	D2_rem = true;
	        	}
	        	
	        	// check if entry.first has been made compound by this split
	        	bool is_last = P.is_last(entry.first);
	        	bool is_first = P.is_first(entry.first);

	        	if( first != nullptr && is_first )
	        	{
	        		//P.update_first(entry.first,first);
	        		//is_last = P.is_last(entry.first);
	        		if( no_new_parts > 1 && is_last )
	        		{
	        			// if true entry.first was not compound before this split
	        			P.insert_C( P.update_return_first(entry.first,first) );
	        		} // else if it was already a compound
	        		else{ P.update_first(entry.first,first); }
	        	}
	        	// if we removed D2 update the pointer to last element of compound
	        	if( D2_rem && is_last ) P.update_last(entry.first,last);
	      	}
	      	else // D2 D11 D12
	      	{
		      	// create new D12 part
		        if(part_D12 != nullptr)
		        { 
		          P.insert(entry.first,part_D12);
		          last = part_D12;
		          no_new_parts++;
		        }
		        // create new D11 part
		        if(part_D11 != nullptr)
		        {
		          P.insert(entry.first,part_D11);
		          if(last == nullptr) { last = part_D11; }
		          no_new_parts++;
		        }
		       	// remove D2 part if it is empty
	        	bool D2_rem = false;
	        	if(entry.first->nodes->size() == 0)
	        	{ 
	        		assert(no_new_parts > 1);
		        	no_new_parts--;
		        	first = entry.first->next;
		        	assert(first == part_D11 || first == part_D12);
		        	P.remove_part(entry.first);
		        	D2_rem = true;
	        	}
		        /////////////////////////////////////////
	        	// check if entry.first has been made compound by this split
	        	bool is_last = P.is_last(entry.first);
	        	bool is_first = P.is_first(entry.first);
	        	if( last != nullptr && is_last )
	        	{
	        		//is_first = P.is_first(entry.first);
	        		if( no_new_parts > 1 && is_first )
	        		{
	        			// if true entry.first was not compound before this split
	        			P.insert_C( P.update_return_last(entry.first,last) );
	        		} // else if it was already a compound
	        		else{ P.update_last(entry.first,last); /*std::cout << "2.1 ";*/}
	        	}
	        	// if we removed D2 update the pointer to last element of compound
	        	if( D2_rem && is_first ){ P.update_first(entry.first,first); /*std::cout << "3 ";*/ }
	   	 	}
	    }

	    if( S.second->first == S.second->second )
	    {
	    	#ifdef VERBOSE
	    	{
	    		std::cout << "S is not a compound block\n";
	    	}
	    	#endif
	    	if( S.second->first->nodes->size() == 1 )
	     	{
	     		#ifdef VERBOSE
	     		{
	     			std::cout << "delete S\n";
	     		}
	     		#endif
	      		P.remove_first_last_part(S.second->first);
	      		delete S.second;
	      	}
	    }
	    // insert B
	    if( B->first == B->second )
	    {
	    	#ifdef VERBOSE
	    	{
	    		std::cout << "B is not a compound block\n";
	    	}
	    	#endif

	    	if( B->first->nodes->size() == 1 )
	    	{
	    		#ifdef VERBOSE
	    		{
	    			std::cout << "delete B\n";
	    		}
	    		#endif
		    	// delete pointers here
		    	P.remove_first_last_part(B->first);
		    	delete B;
	    	}
	    }
 	}
}


/**
 * @brief Sort a Wheeler NFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param NFA input consistent NFA to sort
 */
void partition_refinement_NFA(partition& P, graph& NFA){
	// input check before algorithm execution
	if((NFA.no_nodes() == 0) || (P.give_head()->next == nullptr)) {std::cerr << "Empty input found." << std::endl; exit(-1);}
	// execute sorting algorithm
	PART_REF_NFA(P, NFA);
}

/**
 * @brief Sort an input consistent DFA using the partition refinement algorithm
 * 
 * @param P   input partition
 * @param DFA input consistent DFA to sort
 */
void partition_refinement_DFA(partition& P, graph& DFA){
	// input check before algorithm execution
	if((DFA.no_nodes() == 0) || (P.give_head()->next == nullptr)) {std::cerr << "Empty input found." << std::endl; exit(-1);}
	// execute sorting algorithm
	PART_REF_DFA(P, DFA);
}