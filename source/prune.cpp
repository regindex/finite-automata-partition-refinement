// include partition refinement algorithm
#include "lib/part_ref.h"
#include "internal/utils.hpp"
   
int main(int argc, char** argv)
{
  // initialize algorithm data structures
  partition P; // partition
  graph Aut; // automaton
  std::string out_file, in_file; // out and input file
  uint_t n, base_ind, source;
  bool pruning, dot_format, dummy, suprema, compact;
  // read input from file
  if(argc == 9)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);
    n = std::stoull(argv[3]);
    source = std::stoull(argv[4]);
    dot_format = atoi(argv[5]);
    base_ind = atoi(argv[6]);
    suprema = atoi(argv[7]);
    dummy = atoi(argv[8]);

    #ifdef VERBOSE
    {
      std::cout << "initialize partition..." << std::endl;
    }
    #endif
    
    // initialize the initial partition
    P = partition(sigma_ascii,source);

    #ifdef VERBOSE
    {
      std::cout << "initialize automaton..." << std::endl;
    }
    #endif

    // initialize the automaton
    Aut = graph(n,source);
    // Aut.set_source(source);
    // parse input file
    parse_input_file_prune(in_file,P,Aut,dot_format,base_ind,suprema,dummy); 
    // delete auxiliary data structures
    Aut.delete_counts();
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments" << std::endl; exit(1); }

  #ifdef VERBOSE
  {
    std::cout << "running pruning algorithm..." << std::endl;
  }
  #endif

  // prune input automaton using the partition refinement algorithm
  partition_refinement_pruning(P, Aut);
  // write output pruned automaton to file
  Aut.to_output_pruned(out_file);

  return 0;
}