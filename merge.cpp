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
void parse_dot(std::string input_file, partition& P, graph& Aut, uint_t sc)
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
      Aut.add_edge(origin, dest, label, P);
    }
    // close stream to input file
    input.close();
}  

// simple parser for intermediate file; it reads line by line origin \t destination \t label \n
void parse_intermediate(std::string input_file, partition& P, graph& Aut, uint_t sc, uint_t offset, bool dna)
{
    // open stream to input
    std::cout << "entra\n";
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

      std::cout << out.size() << "\n";
      std::cout << out[0] << " " << out[1] << " " << out[2] << "\n";
      
      if(out.size() != 3){ continue; }

      origin = stoul(out[0]) - sc + offset;
      dest = stoul(out[2]) - sc + offset;
      label = (int)out[1][0];
      std::cout << label << "\n";
      if( dna ){ label = (int)seq_nt6_table[label]; }
      std::cout << label << "\n";
      //label = (int)seq_nt6_table[(int)out[2][0]];

      P.add_node(dest, label);
      Aut.add_edge(origin, dest, label, P);
    }
    // close stream to input file
    input.close();
}  

// simple parser for intermediate file; it reads line by line origin \t destination \t label \n
void get_labels(std::string input_file, std::vector<char>& labels, uint_t sc)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = ' ';
    uint_t dest;
    int label;
    std::vector<std::string> out; 

    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 
      
      if(out.size() != 3){ continue; }

      dest = stoul(out[1]) - sc;
      label = out[2][0];

      labels[dest] = label;
    }
    // close stream to input file
    input.close();
}  
   
int main(int argc, char** argv)
{
  // read command line arguments
  partition P;
  graph Aut;
  std::string out_file, in_file_1, in_file_2;
  uint_t n, sc, source;
  bool dot_format, ascii;
  // read input from file
  if(argc > 8)
  { 
    // set input parameters
    in_file_1 = std::string(argv[1]);
    in_file_2 = std::string(argv[2]);
    out_file = std::string(argv[3]);
    n = std::stoull(argv[4]);
    source = std::stoull(argv[5]);
    sc = atoi(argv[6]);
    ascii = atoi(argv[7]);
    dot_format = atoi(argv[8]);

    #ifdef VERBOSE
    {
      std::cout << "initialize first partition...\n";
    }
    #endif
    // initialize the initial partition
    if( ascii ){ P = partition(sigma_ascii,source); }
    else{        P = partition(sigma,source); }
    // add second source
    P.add_source(source+n);
    P.print_partition();
    #ifdef VERBOSE
    {
      std::cout << "initialize automaton...\n";
    }
    #endif
    // initialize the automaton
    Aut = graph(n*2);

    if( dot_format ){ std::cerr << "not ready yet\n"; exit(1); parse_dot(in_file_1,P,Aut,sc); } // TODO ascii and dna alphabet selection for dot
    else
    {
      // read input using dna alphabet or full ascii alphabet
      if( ascii )
      {
        parse_intermediate(in_file_1,P,Aut,sc,0,false);
        parse_intermediate(in_file_2,P,Aut,sc,n,false);
      }
      else
      {
        parse_intermediate(in_file_1,P,Aut,sc,0,true); 
        parse_intermediate(in_file_2,P,Aut,sc,n,true); 
      }
    }

    Aut.delete_counts();
    P.delete_spoint();

    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  std::cout << "Start sorting infima and suprema automata\n";
  partition_refinement_NFA(P, Aut);

  std::cout << "Store state intervals to file\n";
  Aut.store_state_intervals(out_file, P, n);

  return 0;
}