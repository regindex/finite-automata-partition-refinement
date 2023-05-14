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

// simple parse for dot files; it reads the input line by line
void parse_dot(std::string input_file, partition& P, graph& NFA, uint_t sc)
{
    // open stream to input
    std::ifstream input(input_file);

    std::string line;
    const char delim = ' ';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 

    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 
      if(out.size() < 8){ continue; }

      origin = stoul(out[0].substr(2, out[0].size())) - sc;
      dest = stoul(out[2].substr(1, out[2].size())) - sc;
      label = stoi(out[6]) + 1;

      P.add_node(dest, label);
      NFA.add_edge(origin, dest, label, P);
    }
    // close stream to input file
    input.close();
}  

// simple parser for intermediate file; it reads line by line origin \t destination \t label \n
void parse_intermediate(std::string input_file, partition& P, graph& NFA, uint_t sc)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = '\t';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 

    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 
      
      if(out.size() != 3){ continue; }

      origin = stoul(out[0]) - sc;
      dest = stoul(out[1]) - sc;
      label = (int)seq_nt6_table[(int)out[2][0]];

      P.add_node(dest, label);
      NFA.add_edge(origin, dest, label, P);
    }
    // close stream to input file
    input.close();
}  
   
int main(int argc, char** argv)
{
  // read command line arguments
  partition P;
  graph NFA;
  std::string out_file, in_file;
  uint_t n, sc, origin;
  bool dfa, write_output, dot_format;
  // read input from file
  if(argc > 7)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);
    n = atoll(argv[3]);
    sc = atoi(argv[4]);
    dfa = atoi(argv[5]);
    dot_format = atoi(argv[6]);
    write_output = atoi(argv[7]);

    #ifdef VERBOSE
    {
      std::cout << "initialize first partition...\n";
    }
    #endif
    // set source state label; assume initial state has id zero
    origin = 0;
    // initialize the initial partition
    P = partition(sigma,origin);
    #ifdef VERBOSE
    {
      std::cout << "initialize NFA...\n";
    }
    #endif
    // initialize the NFA
    NFA = graph(n);

    if( dot_format ){ parse_dot(in_file,P,NFA,sc); }
    else{ parse_intermediate(in_file,P,NFA,sc); }

    NFA.delete_counts();
    P.delete_spoint();

    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  // run partition refinement algorithm
  if( dfa )
  {
      std::cout << "Start sorting DFA\n";
      partition_refinement_DFA(P, NFA);

      // print the output
      if( write_output )
      {
        P.to_file(out_file);

        NFA.to_file(std::string(argv[1])+".pruned");
      }
  }
  else
  {
      std::cout << "Start sorting NFA\n";
      partition_refinement_NFA(P, NFA);

      // print the output
      if( write_output )
      {
        P.to_file(out_file);
      }
  }

  return 0;
}