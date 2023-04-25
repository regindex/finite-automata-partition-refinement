// include partition refinement algorithm
#include "lib/part_ref.h"

void tokenize(std::string const &str, const char delim, 
            std::vector<std::string> &out) 
{ 
    // construct a stream from the string 
    std::stringstream ss(str); 
 
    std::string s; 
    while (std::getline(ss, s, delim)) { 
        out.push_back(s); 
    } 
} 
   
int main(int argc, char** argv)
{
  // read command line arguments
  partition P;
  graph NFA;
  std::string out_file = std::string(argv[2]);
  uint_t n = atoll(argv[3]);
  int sc = atoi(argv[4]);
  bool verb = atoi(argv[5]);
  // read input from file
  if(argc > 5)
  {
    std::ifstream input(argv[1]);
    
    if( verb ) std::cout << "initialize first partition...\n";
    // initialize the initial partition
    P = partition(sigma);
    if( verb ) std::cout << "initialize NFA...\n";
    // initialize the NFA
    NFA = graph(n);

    std::string line;
    const char delim = ' ';

    if( verb ){ std::cout << "Edges list:\n"; }
    // very simple dot file parser, TODO improve the parser
    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 
      if(out.size() < 8){ continue; }

      uint_t origin = stoul(out[0].substr(2, out[0].size())) - sc;
      uint_t dest = stoul(out[2].substr(1, out[2].size())) - sc;
      int label = stoi(out[6]) + 1;
      if( verb ) std::cout << origin << " " << dest << " " << label << "\n";

      P.add_node(dest, label);
      NFA.add_edge(origin, dest, label, P);
    }

    input.close();
    NFA.delete_counts();
    P.delete_spoint();

    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  // run partition refinement algorithm
  partition_refinement_NFA(P, NFA, verb);

  // print the output
  P.to_file(out_file);

  return 0;
}