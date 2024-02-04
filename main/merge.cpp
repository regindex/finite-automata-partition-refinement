// include partition refinement algorithm
#include "lib/part_ref.h"
#include "internal/utils.hpp"

void prune_automaton(std::string in_file, std::string out_file,
                     uint_t n, uint_t source, bool dot_format,
                     bool base_ind, bool suprema)
{
    // initialize the initial partition
    partition P = partition(sigma_ascii,source);
    // initialize the automaton
    graph Aut = graph(n);
    Aut.set_source(source);
    // parse input file
    parse_input_file(in_file,P,Aut,dot_format,base_ind,suprema,suprema); 
    // delete auxiliary data structures
    Aut.delete_counts();
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
    // run partition refinement algorithm
    partition_refinement_pruning(P, Aut);
    // write automaton to file
    Aut.to_output_pruned(out_file);
}
   
int main(int argc, char** argv)
{
  // read command line arguments
  partition P;
  graph Aut;
  std::string out_file, in_file;
  uint_t n, source;
  bool dot_format, base_ind;
  // read input from file
  if(argc == 7)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);
    n = std::stoull(argv[3]);
    source = std::stoull(argv[4]);
    dot_format = atoi(argv[5]);
    base_ind = atoi(argv[6]);

    // prune infima automaton
    prune_automaton(in_file,in_file+".infima",n,source,
                    dot_format,base_ind,false);
    // prune suprema automaton
    prune_automaton(in_file,in_file+".suprema",n,source,
                    dot_format,base_ind,true);

    /* Merge and sort infima and suprema automata */
    // initialize the initial partition
    P = partition(sigma_ascii,source);
    // add second source
    P.add_source(source+n);
    // initialize the automaton
    Aut = graph(n*2);
    // parse input files
    parse_input_merge(in_file+".infima",in_file+".suprema",n,P,Aut); 
    // delete auxiliary data structures
    Aut.delete_counts();
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  /* sort Aut automaton using the partition refinement algorithm */
  partition_refinement_Wheeler_automaton(P, Aut);
  /* write output files */
  Aut.store_state_intervals(out_file, P, n);

  return 0;
}