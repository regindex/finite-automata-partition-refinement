#include "lib/partial_order.hpp"
#include <iostream>
#include <queue>
#include <fstream>

template <typename T> 
bool compare(const T& a, const T& b) {
    std::cout << "The used type is not already implemented." << std::endl;
    exit(0);
}

template <> 
bool compare<char>(const char& a, const char& b) {
    if (a == ' ')
        return true;
    if (a <= b)
        return true;
    else 
        return false;
}

template <> 
bool compare<std::string>(const std::string& a, const std::string& b) {
    if(a==b)
        return true;
    if(a == "")
        return true;
    if(b == "")
        return false;
    
    std::size_t a_hash = std::hash<std::string>{}(a), b_hash = std::hash<std::string>{}(b);
    if (a_hash <= b_hash)
        return true;
    else 
        return false;
}

template <typename T> 
bool compare_not_eq(const T& a, const T& b) {
    std::cout << "The used type is not already implemented." << std::endl;
    exit(0);
}

template <> 
bool compare_not_eq<char>(const char& a, const char& b) {
    if (a == ' ')
        return true;
    if (b == ' ')
        return false;
    if (a < b)
        return true;
    else 
        return false;
}

template <> 
bool compare_not_eq<std::string>(const std::string& a, const std::string& b) {
    if(a == "")
        return true;
    if( b== "")
        return false;
    std::size_t a_hash = std::hash<std::string>{}(a), b_hash = std::hash<std::string>{}(b);
    if (a_hash < b_hash)
        return true;
    else 
        return false;
}

OrderCFS::OrderCFS(Graph_T& G)
{
    this->n = G.get_n();
    this->incidenceMatrix.resize(n, std::vector<bool>(n,1));
    //we need to have 1 at [u][v] when u <= v
    //we set the wrong pairs to false and we propagate forward
    std::queue<std::pair<uint_t,uint_t>> visit_q; 

    for (size_t u = 0; u < n; ++u) {
        for (size_t v = 0; v < n; ++v) {
            // lambda(u) > lambda(v)
            // set to false because u !< v
            // reflexive due to total order of the alphabet
            // if the pair is marked we skip it 
            if(incidenceMatrix.at(u).at(v) == 1)
            {
                if( !compare(G.getLabel(u), G.getLabel(v) ))
                {
                    incidenceMatrix.at(u).at(v) = 0;

                    // propagate
                    visit_q.push(std::make_pair(u,v));
                    // BFS visit
                    while(visit_q.size() != 0)
                    {
                        //extract the pair (u1, v1)
                        std::pair<uint_t,uint_t> current_node = visit_q.front(); visit_q.pop();
                        uint_t u1 = current_node.first, v1 = current_node.second;

                        std::vector<std::pair<uint_t,char>> u1_edges, v1_edges;
                        u1_edges = G.getEdgeList(u1);
                        v1_edges = G.getEdgeList(v1);

                        for (size_t u2 = 0; u2 < u1_edges.size(); ++u2) {
                            for (size_t v2 = 0; v2 < v1_edges.size(); ++v2)
                            {
                                auto u2_ = u1_edges[u2]; auto v2_ = v1_edges[v2];
                                //if the pair u2_ v2_ are reached by the same label
                                if( u2_.second != G.getNull() and u2_.second == v2_.second and
                                                            incidenceMatrix.at(u2_.first).at(v2_.first) == 1)
                                {
                                    incidenceMatrix.at(u2_.first).at(v2_.first) = 0;
                                    visit_q.push(std::make_pair(u2_.first,v2_.first));
                                }
                            }
                        }
                    } // end BFS visit
                }
                else{ break; } // this only works if automaton's state ids are sorted
            }
        }
    }
    // set diagonal to 1
    for (size_t i = 0; i < this->n; i++) { incidenceMatrix.at(i).at(i) = 1; }
}

void OrderCFS::printIncidenceMatrix() {
    std::cout << "  ";
    for (size_t i = 0; i < this->n; i++)
    {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    for (size_t i = 0; i < this->n; ++i) {
        std::cout << i << " ";
        for (size_t j = 0; j < this->n; j++) {
            std::cout << this->incidenceMatrix.at(i).at(j) << ' ';
        }
        std::cout << std::endl;    
    }
}

Vbb OrderCFS::getIncidenceMatrix() { return this->incidenceMatrix; }

void OrderCFS::storeOrder(std::string output_file_name) {
    size_t m = 0;
    //iterate through the matrix
    for (size_t i = 0; i < this->n; ++i) {
        for (size_t j = 0; j < this->n; ++j) {
            if(this->incidenceMatrix.at(i).at(j) == 1) {
                ++m;
            }
        }
    }
    std::ofstream output_file;
    output_file.open(output_file_name);
    output_file << this->n << " " << m << std::endl;

    //iterate through the matrix
    for (size_t i = 0; i < this->n; ++i) {
        for (size_t j = 0; j < this->n; ++j) {
            if(this->incidenceMatrix.at(i).at(j) == 1) {
                output_file << i+1 << " " << j+1 << std::endl;
            }
        }
    }
    output_file.close();
}

void OrderCFS::printOrder() {
    size_t m = 0;
    //iterate through the matrix
    for (size_t i = 0; i < this->n; ++i) {
        for (size_t j = 0; j < this->n; ++j) {
            if(this->incidenceMatrix.at(i).at(j) == 1) {
                ++m;
            }
        }
    }
    std::cout << this->n << " " << m << std::endl;

    //iterate through the matrix
    for (size_t i = 0; i < this->n; ++i) {
        for (size_t j = 0; j < this->n; ++j) {
            if(this->incidenceMatrix.at(i).at(j) == 1) {
                std::cout << i+1 << " " << j+1 << std::endl;
            }
        }
    }
}

// Function to perform DFS and topological sorting
void OrderCFS::topologicalSortUtil(size_t v, std::vector<bool>& visited,
                                             std::vector<uint_t>& stack) {
    // Mark the current node as visited
    visited[v] = true;

    // Recur for all adjacent vertices
    for(size_t i = v+1; i < this->n; ++i)
    {
        if(incidenceMatrix.at(v).at(i))
            if(!visited[i])
                topologicalSortUtil(i, visited, stack);
    }

    // Push current vertex to stack which stores the result
    stack.push_back(v);
}

std::vector<uint_t> OrderCFS::topologicalSort()
{
    // stack to store the result
    std::vector<uint_t> stack;
    // vector storing the visited nodes
    std::vector<bool> visited(this->n,false);

    // Topological Sort starting from source node
    for(size_t i = 0; i < this->n; ++i) {
        if(!visited[i])
            topologicalSortUtil(i, visited, stack);
    }

    // Print contents of stack
    /*
    for (int64_t i = this->n-1; i >= 0; --i)
        std::cout << stack[i]+1 << " ";
    std::cout << std::endl;
    */

    return stack;
}

Vbb OrderCFS::getTransitiveReduction()
{
    std::vector<uint_t> top_order = topologicalSort();
    //top_order = std::vector<uint_t>{8,7,6,5,4,3,2,1,0};
    Vbb reachable;
    // reachable sets initialization
    reachable.resize(this->n, std::vector<bool>(this->n,0));
    for(size_t i=0;i<this->n;++i)
        reachable.at(i).at(i) = true;

    // process every node v in reverse topological order
    for(size_t i = 0; i < this->n; ++i)
    {
        uint_t v = top_order[i];
        // std::cout << "processing i: " << top_order[i]+1 << std::endl;
        // process every edge vw in topological order
        for(size_t w = top_order[i]+1; w < this->n; ++w)
        {
            // check if the edge actually exists 
            if(this->incidenceMatrix.at(v).at(w))
            {
                // if w is not in the reachable set of v, we add edge vw
                // in the transitive reduction, and add w to the reachable set of v
                if(not reachable.at(v).at(w))
                {
                    //std::cout << "edge: " << v+1 << "," << w+1 << std::endl;
                    reachable.at(w).at(v) = true; // store trasitive reduction in the lower triangular matrix
                    for(size_t y=w; y<this->n; ++y)
                        if(reachable.at(w).at(y)){ reachable.at(v).at(y) = true; }
                }
            }
        }
    }
    // store the lower triangular matrix in the upper one
    for(size_t i=0;i<this->n;++i)
        for(size_t j=i;j<this->n;++j)
        {
            if(i==j){ reachable.at(i).at(j) = false; }
            else
            {
                if(reachable.at(j).at(i))
                    { reachable.at(i).at(j) = true; reachable.at(j).at(i) = false; }
                else{ reachable.at(i).at(j) = false; }
            }
        }

    return reachable;
}

/* Vbb OrderCFS::getTransitiveReduction() {
    Vbb trans(this->incidenceMatrix);

    for(int k = 0; k<this->n; ++k)
        for(int i = 0; i<this->n; ++i)
            for(int j = 0; j<this->n; ++j)
                if( trans.at(k).at(i) && trans.at(i).at(j) && i != k && i != j) trans.at(k).at(j) = 0;
    return trans;
}
void OrderCFS::setTransitiveReduction() {
    Vbb trans(this->incidenceMatrix);

    for(int k = 0; k<this->n; ++k)
        for(int i = 0; i<this->n; ++i)
            for(int j = 0; j<this->n; ++j)
                if( trans.at(k).at(i) && trans.at(i).at(j) && i != k && i != j) trans.at(k).at(j) = 0;
    this->incidenceMatrix = trans;
} */

void OrderCFS::printTransitiveReduction() {
    //transitive reduction of incidence matrix
    Vbb T(getTransitiveReduction());

    std::cout << "  ";
    for (size_t i = 0; i < this->n; i++)
    {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    for (size_t i = 0; i < this->n; ++i) {
        std::cout << i << " ";
        for (size_t j = 0; j < this->n; j++) {
            std::cout << T.at(i).at(j) << ' ';
        }
        std::cout << std::endl;    
    }
}

void OrderCFS::printTransitiveReductionTransitions() {
    //transitive reduction of incidence matrix
    Vbb T(getTransitiveReduction());

    for (size_t i = 0; i < this->n; ++i) {
        for (size_t j = 0; j < this->n; j++)
        {
            if(T.at(i).at(j))
                std::cout << i+1 << " -> " << j+1 << std::endl;
        }  
    }
}
