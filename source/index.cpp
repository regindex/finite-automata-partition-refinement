// include partition refinement algorithm
#include "lib/part_ref.h"
#include "lib/partial_order.hpp"
#include "internal/utils.hpp"
#include "external/mpc.hpp"
   
int main(int argc, char** argv)
{
  // initialize algorithm data structures
  partition P; // partition
  graph Aut; // automaton
  std::string out_file, in_file; // out and input file
  uint_t n, base_ind, source;
  bool dot_format, compact;
  // read input from file
  if(argc == 3)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);

    #ifdef VERBOSE
    {
      std::cout << "initialize first partition...\n";
    }
    #endif
    
    //
    std::pair<uint_t,uint_t> n_source = get_n_source_graphML(in_file);
    // initialize the initial partition
    P = partition(sigma_ascii,n_source.second);

    #ifdef VERBOSE
    {
      std::cout << "initialize automaton...\n";
    }
    #endif

    // initialize the automaton
    Aut = graph(n_source.first,source);
    // Aut.set_source(source);
    // parse input file
    parse_input_graphML(in_file,P,Aut); 
    // delete ausiliary data structures
    Aut.delete_counts();
    P.delete_spoint();
    // search first compound block
    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  /* sort Aut automaton using the partition refinement algorithm */
  partition_refinement_Wheeler_automaton(P, Aut);
  /* write partition file */
  P.to_file(out_file+".mapping");

  /* extract the remapped sorted automaton */
  IC_graph sorted; 
  Aut.get_sorted_automaton(sorted,P);
  #ifdef VERBOSE
  {
    std::cout << "# PARTITION #" << std::endl;
    P.print_partition();
    std::cout << "# CFS GRAPH #" << std::endl;
    sorted.print();
  }
  #endif
  P.clear(); Aut.clear();

  /* compute the CFS partial order */
  OrderCFS partial_order_CFS(sorted);
  /* compute the chain decomposition given the hasse diagram */
  auto cdec = get_chain_decomposition<uint_t>(partial_order_CFS.getTransitiveReduction());

  #ifdef VERBOSE
  {
    std::cout << "# HASSE DIAGRAM #" << std::endl;
    partial_order_CFS.printTransitiveReductionTransitions();
    std::cout << "# CHAIN DECOMPOSITION #" << std::endl;
    for(size_t i=0;i<cdec.size();++i)
    {
      std::cout << "chain " << i << ": ";
      for(auto &e: cdec[i]){ std::cout << e << " "; }
      std::cout << std::endl;
    }
  }
  #endif

  /* store annotated graph */
  sorted.store_graphML(out_file+".graphML",cdec);

  return 0;
}