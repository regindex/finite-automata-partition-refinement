// include NFA header
#include "internal/NFA.hpp"

typedef NFA_unidirectional_in_unlabelled graph; 

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
  std::string we_order = std::string(argv[2]);
  uint_t n = atoll(argv[3]);
  int sc = atoi(argv[4]);
  bool verb = atoi(argv[5]);
  // read input from file
  if(argc > 5)
  {
    std::ifstream input(argv[1]);
    
    if( verb ) std::cout << "initialize NFA...\n";
    // initialize the NFA
    NFA = graph(n);

    std::string line;
    const char delim = ' ';

    if( verb ){ std::cout << "Edges list:\n"; }
    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 
      if(out.size() < 8){ continue; }

      uint_t origin = stoul(out[0].substr(2, out[0].size())) - sc;
      uint_t dest = stoul(out[2].substr(1, out[2].size())) - sc;
      int label = stoi(out[6]) + 1;
      if( verb ) std::cout << origin << " " << dest << " " << label << "\n";

      NFA.add_edge(origin, dest, label);
    }

    input.close();
  }
  else{ std::cerr << "invalid no. arguments\n"; exit(1); }

  // init isa
  std::vector<uint_t> ISA(n,0);

  std::ifstream output(argv[2]);
  uint_t i = 0;
  uint_t curr;

  while( true )
  {
    output.read(reinterpret_cast<char *>(&curr), sizeof(curr));
    if( output.gcount() == 0) break;
    if( curr == U_MAX ){ ++i; continue; }

    ISA[curr] = i;
  }

  output.clear();
  output.seekg(0);

  uint_t max = 0, prev_max = 0;
  char curr_label = 0;
  i = 0;
  while( true )
  {
    output.read(reinterpret_cast<char *>(&curr), sizeof(curr));
    if( output.gcount() == 0) break;
    if( curr == U_MAX )
    {
      i++;
      prev_max = max;
      max = 0;
      continue;
    }
    else
    {
      if( NFA.at(curr)->label != curr_label )
      {
        curr_label = NFA.at(curr)->label;
        max = prev_max = 0;
      }
    }

    if( verb )
    {
      std::cout << "######curr: " << curr << "\n";
      std::cout << "i: " << i << "\n";
      std::cout << "curr label: " << int(curr_label) << "\n";
      std::cout << "curr max: " << max << "\n";
      std::cout << "prev max: " << prev_max << "\n";
    }

    for(uint_t j=0;j<NFA.at(curr)->in.size();++j){
      uint_t curr_isa = ISA[NFA.at(curr)->in[j]];
      if( verb ) std::cout << "current isa: " << curr_isa << "\n";
      if( curr_isa > max ){ max = curr_isa; }

      if( max < prev_max ){ std::cout << "Not Wheeler in: " << i << "\n"; exit(1); }
    }
    
  }

  std::cout << "OK! it is a Wheeler order!" << "\n";

  return 0;
}