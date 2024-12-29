#ifndef PartialOrder_H
#define PartialOrder_H

#include "internal/finite_automaton.hpp"
#include <string>

typedef std::vector<std::vector<bool>> Vbb;
typedef Automata_input_consistent Graph_T;

class OrderCFS {
    private:
        size_t n;
        Vbb incidenceMatrix;

        std::vector<uint_t> topologicalSort();
        void topologicalSortUtil(size_t v, std::vector<bool>& visited,
                                           std::vector<uint_t>& stack);
    public:
        OrderCFS(Graph_T& G);
        void storeOrder(std::string output_file_name);
        void printOrder(); 

        void printIncidenceMatrix();
        Vbb  getIncidenceMatrix();

        void printTransitiveReduction();
        void printTransitiveReductionTransitions();
        Vbb  getTransitiveReduction();
};

#endif