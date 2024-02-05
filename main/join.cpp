// include util functions
#include "internal/definitions.hpp"
   
int main(int argc, char** argv)
{
    // initialize command line arguments
    std::string infima, suprema, out_file;
    uint_t n;
    bool write_in;
    // read input from file
    if(argc == 6)
    { 
        // set input parameters
        infima = std::string(argv[1]);
        suprema = std::string(argv[2]);
        out_file = std::string(argv[3]);
        n = std::stoull(argv[4]);
        write_in = std::stoull(argv[5]);

        // open input infima automaton
        std::ifstream infimaL(infima+".L");
        std::ifstream infimaIn(infima+".in");
        std::ifstream infimaOut(infima+".out");
        // open input suprema automaton
        std::ifstream supremaL(suprema+".L");
        std::ifstream supremaIn(suprema+".in");
        std::ifstream supremaOut(suprema+".out");
        // open output infsup automaton
        std::ofstream infsupL(out_file+".L");
        std::ofstream infsupIn(out_file+".in");
        std::ofstream infsupOut(out_file+".out");

        // init in and out degree variables
        uint_t in_degree, out_degree;
        std::string bwt_char(n,0);
        // iterate over all nodes
        for(uint_t i=0;i<n;++i)
        {
            infimaOut >> out_degree;
            infimaL.read(&bwt_char[0],out_degree);
            infsupL.write(&bwt_char[0],out_degree);
            infsupOut << std::string(out_degree,'0') << '1';

            supremaOut >> out_degree;
            supremaL.read(&bwt_char[0],out_degree);
            infsupL.write(&bwt_char[0],out_degree);
            infsupOut << std::string(out_degree,'0') << '1';
        }
        // write last .in file
        infsupIn << std::string(2,'1');
        if(write_in)
        {
            for(uint_t i=0;i<(n-1)*2;++i)
            infsupIn << "01";
        }

        // close input/output files
        infimaL.close(); infimaIn.close(); infimaOut.close();
        supremaL.close(); supremaIn.close(); supremaOut.close();
        infsupL.close(); infsupIn.close(); infsupOut.close();
    }
    else{ std::cerr << "invalid no. arguments\n"; exit(1); }

    return 0;
}