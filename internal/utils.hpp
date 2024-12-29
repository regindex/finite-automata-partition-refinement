#ifndef UTILS_H
#define UTILS_H

#include "internal/definitions.hpp"
#include "internal/partition.hpp"
#include "internal/finite_automaton.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include "libxml/xmlreader.h"

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

typedef std::vector<std::tuple<uint_t,uint_t,char>> edge_list;
/*  
    Function that, given an automaton A and a list of edges violating 
    the input consistency property, ensures input consistency in A by splitting 
    all states that are not input-consistent.
*/
void enforce_input_consistency(partition& P, graph& Aut, edge_list& bad_edges) 
{
    std::unordered_map<uint_t,uint_t> mapping;
    for(size_t i=0;i<bad_edges.size();++i)
    {
        uint_t new_id = Aut.no_nodes();
        Aut.add_node();

        Aut.add_label(new_id, std::get<2>(bad_edges[i]));
        P.add_node(new_id, std::get<2>(bad_edges[i]));
        Aut.add_edge(std::get<0>(bad_edges[i]), new_id, std::get<2>(bad_edges[i]), P);

        mapping[new_id] = std::get<1>(bad_edges[i]);
    }
    // connect the new states in the automaton
    for(auto i=mapping.begin();i!=mapping.end();++i)
    {
        auto edges = Aut.at(i->second);
        for(size_t i_=0;i_<edges->out.size();++i_)
        {
            uint_t origin = i->first;
            uint_t dest = edges->out.at(i_);
            //std::cout << "add edge: " << origin << " " << dest << " " << Aut.give_labels()->at(dest) << std::endl;

            Aut.add_edge(origin,dest,Aut.give_labels()->at(dest),P);
        }
    }
}
/* 
   simple XML parser for reading general automata stored in graphML format
   and retriving the initial state the largest node id 
*/
std::pair<uint_t,uint_t> get_n_source_graphML(std::string input_file)
{
    // initialize xml parser
    xmlTextReaderPtr reader = xmlReaderForFile(input_file.c_str(), nullptr, 0);
    // initialize n (largest node id), and initialState (source state)
    unsigned long long initialState = 0, n = 0;

    // parse the XML file incrementally
    while (xmlTextReaderRead(reader) == 1)
    {
        // Get the type of node (element or text)
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
        {
            // Get the name of the element
            const xmlChar* nodeName = xmlTextReaderLocalName(reader);

            // Parse <graph> for the initial state
            if (xmlStrEqual(nodeName, (const xmlChar*)"graph"))
            {
                // Read <data> elements inside <graph> to find the initial state
                while (xmlTextReaderRead(reader) == 1)
                {
                    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT &&
                        xmlStrEqual(xmlTextReaderLocalName(reader), (const xmlChar*)"data"))
                    {
                        const xmlChar* key = xmlTextReaderGetAttribute(reader, (const xmlChar*)"key");
                        if (xmlStrEqual(key, (const xmlChar*)"initial_state"))
                        {
                            const xmlChar* value = xmlTextReaderReadString(reader);
                            initialState = std::stoull(reinterpret_cast<const char*>(value));
                            xmlFree((void*)value);
                            break;
                        }
                    }
                    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
                        xmlStrEqual(xmlTextReaderLocalName(reader), (const xmlChar*)"graph")) {
                        break;
                    }
                }
            }

            // Check if the node is a node or an edge
            if (xmlStrEqual(nodeName, (const xmlChar*)"node"))
            {
                // Process node data
                const xmlChar* nodeId = xmlTextReaderGetAttribute(reader, (const xmlChar*)"id");
                //std::cout << "Node ID: " << reinterpret_cast<const char*>(nodeId) << std::endl;
                n = std::max(n,std::stoull(reinterpret_cast<const char*>(nodeId)));
            }

            // stop parsing when finding the first edge
            if (xmlStrEqual(nodeName, (const xmlChar*)"edge")) { break; }
        }
    }

    xmlFreeTextReader(reader);

    if(n > 4294967295 and sizeof(uint_t) < 8)
    {
        std::cerr << "There are more than 2^32-1 states in the automaton!"
                  << "Please use the 8 byte compiled binary. exiting..."
                  << std::endl;
        exit(1);
    }

    return std::make_pair(n+1,initialState);
}
/* simple parser for reading general automata stored in graphML format, and 
   encoding it using our custom (internal memory) representation */
void parse_input_graphML(std::string input_file, partition& P, graph& Aut)
{
    // initialize xml parser
    xmlTextReaderPtr reader = xmlReaderForFile(input_file.c_str(), nullptr, 0);
    // initialize needed variables
    std::string source, target, label;
    uint_t source_, target_;
    int label_;
    // edges breaking input-consistency
    edge_list bad_edges;
    // init labels vector
    Aut.init_label_vector();

    // parse the XML file incrementally
    while (xmlTextReaderRead(reader) == 1)
    {
        // Search for nodes/edges tags
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
        {
            const xmlChar* nodeName = xmlTextReaderLocalName(reader);

            // check if the node is an edge
            if (xmlStrEqual(nodeName, (const xmlChar*)"edge"))
            {
                // read the edge
                const xmlChar* source = xmlTextReaderGetAttribute(reader, (const xmlChar*)"source");
                const xmlChar* target = xmlTextReaderGetAttribute(reader, (const xmlChar*)"target");
                const xmlChar* label = nullptr;

                // Read edge's label
                while (xmlTextReaderRead(reader) == 1)
                {
                    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
                    {
                        const xmlChar* key = xmlTextReaderLocalName(reader);
                        if (xmlStrEqual(key, (const xmlChar*)"data"))
                        {
                            const xmlChar* keyVal = xmlTextReaderGetAttribute(reader, (const xmlChar*)"key");
                            if (xmlStrEqual(keyVal, (const xmlChar*)"label"))
                            {
                                label = xmlTextReaderReadString(reader);
                            }
                        }
                    }

                    // stop when reaching the end of the edge tag
                    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT && 
                                                            xmlStrEqual(nodeName, (const xmlChar*)"edge")) {
                        break;
                    }
                }

                source_ = std::stoull(reinterpret_cast<const char*>(source));
                target_ = std::stoull(reinterpret_cast<const char*>(target));
                std::string labelStr = reinterpret_cast<const char*>(label);
                label_ = static_cast<int>(labelStr[0]) + 1;
                /* std::cout << "Edge: " << source_ << " -> " 
                          << target_ << ", label: "
                          << label_ << std::endl; */

                // add ingoing label and check for input-consistency
                if( Aut.add_label(target_, label_) )
                {
                  // add new node and edge
                  P.add_node(target_, label_);
                  Aut.add_edge(source_, target_, label_, P);
                }
                else
                    bad_edges.push_back(std::make_tuple(source_,target_,label_));

                // free the dynamically allocated strings
                xmlFree((void*)source);
                xmlFree((void*)target);
                xmlFree((void*)label);
            }
        }
    }

    // enforce input consistency
    if(bad_edges.size() > 0)
        enforce_input_consistency(P,Aut,bad_edges);

    // free xml parser
    xmlFreeTextReader(reader);
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
    // edges breaking input-consistency
    edge_list bad_edges;
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
          origin = stoull(out[0].substr(2, out[0].size())) - base_ind; 
          dest = stoull(out[2].substr(1, out[2].size())) - base_ind;   
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

        // add ingoing label and check for input-consistency
        if( Aut.add_label(dest, label) )
        {
          // add new node and edge
          P.add_node(dest, label);
          Aut.add_edge(origin, dest, label, P);
        }
        else
            bad_edges.push_back(std::make_tuple(origin,dest,label));
    }

    // close stream to input file
    input.close();

    // enforce input consistency
    if(bad_edges.size() > 0)
        enforce_input_consistency(P,Aut,bad_edges);
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