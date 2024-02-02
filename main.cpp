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
void parse_intermediate(std::string input_file, partition& P, graph& Aut, uint_t sc, bool dna, bool invert)
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
      
      if(out.size() != 3){ continue; }

      origin = stoul(out[0]) - sc;
      dest = stoul(out[1]) - sc;
      label = (int)out[2][0];

      if( dna )
      {
        if(invert){ label = (int)inv_seq_nt6_table[label]; } 
        else{ label = (int)seq_nt6_table[label]; }
      }
      else if( invert ){ label = 127 - label; }

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
  std::string out_file, in_file;
  uint_t n, sc, source;
  bool pruning, write_output, dot_format, ascii, suprema;
  // read input from file
  if(argc > 10)
  { 
    // set input parameters
    in_file = std::string(argv[1]);
    out_file = std::string(argv[2]);
    n = std::stoull(argv[3]);
    source = std::stoull(argv[4]);
    sc = atoi(argv[5]);
    pruning = atoi(argv[6]);
    suprema = atoi(argv[7]);
    ascii = atoi(argv[8]);
    dot_format = atoi(argv[9]);
    write_output = atoi(argv[10]);

    #ifdef VERBOSE
    {
      std::cout << "initialize first partition...\n";
    }
    #endif
    // initialize the initial partition
    if( ascii ){ P = partition(sigma_ascii,source); }
    else{        P = partition(sigma,source); }
    #ifdef VERBOSE
    {
      std::cout << "initialize automaton...\n";
    }
    #endif
    // initialize the automaton
    Aut = graph(n);

    if( dot_format ){ parse_dot(in_file,P,Aut,sc); } // TODO ascii and dna alphabet selection for dot
    else
    {
      //ascii = !ascii;
      parse_intermediate(in_file,P,Aut,sc,!ascii,suprema); 
      //if( ascii ){  parse_intermediate(in_file,P,Aut,sc,false); }
      //else{         parse_intermediate(in_file,P,Aut,sc,true); }
    }

    Aut.delete_counts();
    P.delete_spoint();

    P.set_first_C_block();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  // run partition refinement algorithm
  if( pruning )
  {
      partition_refinement_DFA(P, Aut);

      // print the output
      if( write_output )
      {
          // write partition to file
          // P.to_file(out_file);

          //if( dot_format )
          //{
          //    Aut.to_dot(out_file+".pruned");
          //}
          //else
          //{
          std::vector<char> labels(n, '$');
          get_labels(in_file,labels,sc);
          //Aut.to_intermediate(out_file+".pruned", labels);
          Aut.to_intermediate(out_file, labels);
          //}
      }
  }
  else
  {
      std::cout << "Start sorting Wheeler automaton using Partition refinement algorithm\n";
      partition_refinement_NFA(P, Aut);

      // print the output
      if( write_output )
      {
        P.to_file(out_file);
      }
  }

  return 0;
}