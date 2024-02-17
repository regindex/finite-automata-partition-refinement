// include partition refinement algorithm
#include "lib/part_ref.h"
#include "internal/utils.hpp"
   
int main(int argc, char** argv)
{
  // read command line arguments
  partition P;
  graph Aut;
  std::string out_file, infima_file, 
                       suprema_file;
  uint_t n, source;
  bool dot_format, base_ind, print_inter,
       compact;
  // read input from file
  if(argc == 8)
  { 
    // set input parameters
    infima_file = std::string(argv[1]);
    suprema_file = std::string(argv[2]);
    out_file = std::string(argv[3]);
    n = std::stoull(argv[4]);
    source = std::stoull(argv[5]);
    print_inter = atoi(argv[6]);
    compact = atoi(argv[7]);

    /* Merge and sort infima and suprema automata */
    // initialize the initial partition
    P = partition(sigma_ascii,source);
    // add second source
    P.add_source(source+n);
    // initialize the automaton
    Aut = graph(n*2);
    // parse input files
    parse_input_merge(infima_file,suprema_file,n,P,Aut); 
    // delete auxiliary data structures
    Aut.delete_counts_merge(n);
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  /* sort Aut automaton using the partition refinement algorithm */
  partition_refinement_Wheeler_automaton(P, Aut);
  /* write output files */
  if( print_inter )
    Aut.store_state_intervals(out_file, P, n);
  else
  {
    //Aut.to_output_infsup_Wheeler(out_file + ".dot", P, compact);
    Aut.to_output_Wheeler(out_file, P, compact);
    P.to_file(out_file+".mapping");
  }

  return 0;
}