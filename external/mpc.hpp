#ifndef MPC_HPP
#define MPC_HPP

#include "mpc/pflow.h"
#include "mpc/naive.h"
#include "mpc/graph.h"
#include <string>
#include <fstream>
#include <memory>
#include <utility>

Graph* read_graph2(std::string filename) {
	std::ifstream input(filename);
	if(!input.good()) {
		std::cerr << "Failed to open " << filename << " for reading\n";
	}
	int n, m;
	input >> n >> m;
	Graph *g = new Graph(n);
	for(int i=0; i<m; i++) {
		int a, b;
		input >> a >> b;
		g->add_edge(a, b);
	}
	input.close();
	return g;
}

Graph* read_graph(std::vector<std::vector<bool>> &adj) {
	Graph *g = new Graph(adj.size());
    
	for(int i=0; i<adj.size(); ++i) {
        for(int j=0; j<adj.size(); ++j) {
            if(i != j) {
                if(adj.at(i).at(j)) {
                    g->add_edge(i+1,j+1);
                }
            }   
        }
	}
    
    return g;
}

int solve_mpc(std::vector<std::vector<bool>> adj, std::string output_path) {

	std::unique_ptr<Graph> g;
    g = std::make_unique<Graph>(*read_graph(adj));
    
    std::unique_ptr<Flowgraph<Edge::Minflow>> rg;
    rg = pflowk2(*g);
	auto cover = minflow_reduction_path_recover_faster(*rg);
    assert(is_valid_cover(cover, *g));
    int cover_len = 0;
    for(auto &u:cover)
        cover_len += u.size();

    if(output_path != "") {
        std::ofstream out(output_path);
        if(!out.good()) {
            std::cerr << "Failed to open " << output_path << " for writing\n";
            exit(1);
        }
        out << cover.size() << "\n";
        for(auto &path:cover) {
            for(auto &u:path) {
                out << u << " ";
            }
            out << "\n";
        }
        out.close();
    }
    return cover.size();
}

template<typename T>
std::vector<std::vector<T>> get_chain_decomposition(std::vector<std::vector<bool>> graph)
{
    std::vector<std::vector<T>> chain_dec;

    std::unique_ptr<Graph> g;
    g = std::make_unique<Graph>(*read_graph(graph));
    
    std::unique_ptr<Flowgraph<Edge::Minflow>> rg;
    rg = pflowk2(*g);
    auto cover = minflow_reduction_path_recover_faster(*rg);
    assert(is_valid_cover(cover, *g));

    for(auto &path:cover)
    {
        std::vector<T> chain;
        for(auto &u:path)
            chain.push_back(u);
        chain_dec.push_back(chain);
    }

    return chain_dec;
}

#endif