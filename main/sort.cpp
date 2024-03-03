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
  bool dot_format, compact;
  // read input from file
  if(argc == 8)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);
    n = std::stoull(argv[3]);
    source = std::stoull(argv[4]);
    base_ind = atoi(argv[5]);
    dot_format = atoi(argv[6]);
    compact = atoi(argv[7]);

    #ifdef VERBOSE
    {
      std::cout << "initialize first partition...\n";
    }
    #endif
    
    // initialize the initial partition
    P = partition(sigma_ascii,source);

    #ifdef VERBOSE
    {
      std::cout << "initialize automaton...\n";
    }
    #endif

    // initialize the automaton
    Aut = graph(n,source);
    // Aut.set_source(source);
    // parse input file
    parse_input_file(in_file,P,Aut,dot_format,base_ind); 
    // delete ausiliary data structures
    Aut.delete_counts();
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  /* sort Aut automaton using the partition refinement algorithm */
  partition_refinement_Wheeler_automaton(P, Aut);
  /* write output files */
  Aut.to_output_Wheeler(out_file, P, compact);
  P.to_file(out_file+".mapping");

  return 0;
}