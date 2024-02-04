#ifndef UTILS_H
#define UTILS_H

#include "internal/definitions.hpp"
#include "internal/partition.hpp"
#include "internal/automaton.hpp"

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
void parse_input_file(std::string input_file, partition& P, graph& Aut, bool dot,
                      uint_t base_ind, bool invert = false, bool dummy = false)
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

      if( dot )
      {
        // skip lines not encoding an edge in the dot file
        if(out.size() < 8){ continue; }
        // read an edge in the dot file
        origin = stoull(out[0].substr(2, out[0].size())) - base_ind;
        dest = stoull(out[2].substr(1, out[2].size())) - base_ind;
        label = stoi(out[6]);
      }
      else
      {
        // skip lines not encoding an edge
        if(out.size() != 3){ continue; }
        // read an edge
        origin = stoull(out[0]) - base_ind;
        dest = stoull(out[2]) - base_ind;
        label = (int)out[1][0];
      }

      // if we are running the pruning algorithm store the labels
      //if( pruning )
      Aut.add_label(dest, label);
      // if we are pruning the suprema strings automaton
      if( invert ){ label = 127 - label; }

      // add new node and edge
      P.add_node(dest, label);
      Aut.add_edge(origin, dest, label, P);
    }

    if( invert and dummy )
      Aut.add_dummy_state(P);
    // close stream to input file
    input.close();
}  


/* simple parser for intermediate file; it reads line by line origin \t destination \t label \n */
void parse_input_merge(std::string input_infima, std::string input_suprema, 
                       uint_t n, partition& P, graph& Aut)
{
    // open stream to input
    std::ifstream infima(input_infima);
    std::ifstream suprema(input_suprema);
    std::string line;
    const char delim = ' ';
    uint_t origin, dest;
    int label;
    std::vector<std::string> out; 

    // parse infima automaton
    while(std::getline(infima, line))
    {
      std::vector<std::string> out; 
      tokenize(line, delim, out); 

      // skip lines not encoding an edge
      if(out.size() != 3){ continue; }
      // read an edge
      origin = stoull(out[0]);
      dest = stoull(out[2]);
      label = (int)out[1][0];

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

      // skip lines not encoding an edge
      if(out.size() != 3){ continue; }
      // read an edge
      origin = stoull(out[0]) + n;
      dest = stoull(out[2]) + n;
      label = (int)out[1][0];

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