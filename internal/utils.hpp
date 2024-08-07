#ifndef UTILS_H
#define UTILS_H

#include "internal/definitions.hpp"
#include "internal/partition.hpp"
#include "internal/finite_automaton.hpp"

/* parse an edge line */
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

/* simple parser for intermediate file; it reads line by line origin \t destination \t label \n */
void parse_input_file(std::string input_file, partition& P, graph& Aut, bool dot, uint_t base_ind)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = ' ';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 
    // init labels vector
    Aut.init_label_vector();

    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 

      if( dot )
      {
        // skip lines not encoding an edge in the dot file
        if(out.size() < 8){ continue; }
        // read an edge in the dot file
        origin = stoull(out[0].substr(2, out[0].size())) - base_ind; //- base_ind;
        dest = stoull(out[2].substr(1, out[2].size())) - base_ind;   //- base_ind;
        label = stoi(out[6]) + 1;
      }
      else
      {
        // skip lines not encoding an edge
        if(out.size() != 3){ continue; }
        // read an edge
        origin = stoull(out[0]) - base_ind; // - base_ind;
        dest = stoull(out[2]) - base_ind;   // - base_ind;
        label = stoi(out[1]) + 1;
      }

      // if we are running the pruning algorithm store the labels
      Aut.add_label(dest, label);

      // add new node and edge
      P.add_node(dest, label);
      Aut.add_edge(origin, dest, label, P);
    }

    // close stream to input file
    input.close();
}  

/* simple parser for intermediate file; it reads line by line origin \t destination \t label \n */
void parse_input_file_prune(std::string input_file, partition& P, graph& Aut, bool dot,
                            uint_t base_ind, bool invert = false, bool dummy = false)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = ' ';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 
    // init labels vector
    Aut.init_label_vector();
    // compute in nodes
    std::vector<std::unordered_set<uint_t>> in_nodes;
    in_nodes.resize(Aut.no_nodes());

    while(std::getline(input, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 

      if( dot )
      {
        // skip lines not encoding an edge in the dot file
        if(out.size() < 8){ continue; }
        // read an edge in the dot file
        origin = stoull(out[0].substr(2, out[0].size())) - base_ind; //- base_ind;
        dest = stoull(out[2].substr(1, out[2].size())) - base_ind;   //- base_ind;
        label = stoi(out[6]) + 1;
      }
      else
      {
        // skip lines not encoding an edge
        if(out.size() != 3){ continue; }
        // read an edge
        origin = stoull(out[0]) - base_ind; // - base_ind;
        dest = stoull(out[2]) - base_ind;   // - base_ind;
        label = stoi(out[1]) + 1;
      }

      // if we are running the pruning algorithm store the labels
      int to_prune = Aut.assign_remove_label(dest, label, invert);
      // if we are pruning the suprema strings automaton
      if( invert ){ label = sigma_ascii - label - 1; }

      // add new node and edge
      P.add_node_prune(dest, to_prune, label, invert);
      //P.add_node_prune(dest, label);
      // Aut.add_edge(origin, dest, label, P);
      if(to_prune > -1)
      {
        if(to_prune > 0)
          in_nodes[dest].clear();
        in_nodes[dest].insert(origin);
      }
    }

    // insert not pruned edges in the automaton
    for(uint_t dest=0;dest<Aut.no_nodes();dest++)
    {
      for (const auto& origin: in_nodes[dest])
      {
        int label = (*Aut.give_labels())[dest];
        if(invert){ label = sigma_ascii - label - 1; }
        //std::cout << "add: " << origin << " ->(" << label << ") " << dest << std::endl;
        Aut.add_edge(origin, dest, label, P);
      }
    }
    // insert dummy state for computing suprema strings automaton
    if(invert and dummy)
      Aut.add_dummy_state(P);
    // close stream to input file
    input.close();
} 


/* simple parser for intermediate file; it reads line by line origin \t destination \t label \n */
void parse_input_file_merge(std::string input_infima, std::string input_suprema, uint_t n,
                       partition& P, graph& Aut)
{
    // open stream to input
    std::ifstream infima(input_infima);
    std::ifstream suprema(input_suprema);
    std::string line;
    const char delim = ' ';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 
    // init labels vector
    Aut.init_label_vector();

    // parse infima automaton
    while(std::getline(infima, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 

      // skip lines not encoding an edge in the dot file
      if(out.size() < 8){ continue; }
      // read an edge in the dot file
      origin = stoull(out[0].substr(2, out[0].size())) - 1; // - 1;
      dest = stoull(out[2].substr(1, out[2].size())) - 1;   // - 1;
      label = stoi(out[6]) + 1;

      // if we are running the pruning algorithm store the labels
      Aut.add_label(dest, label);

      // add new node and andge
      P.add_node(dest, label);
      Aut.add_edge(origin, dest, label, P);
    }
    // parse infima automaton
    while(std::getline(suprema, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 

      // skip lines not encoding an edge in the dot file
      if(out.size() < 8){ continue; }
      // read an edge in the dot file
      origin = stoull(out[0].substr(2, out[0].size())) + n - 1; // - 1;
      dest = stoull(out[2].substr(1, out[2].size())) + n - 1;   // - 1;
      label = stoi(out[6]) + 1;

      // if we are running the pruning algorithm store the labels
      Aut.add_label(dest, label);

      // add new node and edge
      P.add_node(dest, label);
      Aut.add_edge(origin, dest, label, P);
    }

    // close stream to input file
    infima.close(); suprema.close();
}


#endif /* UTILS_H */