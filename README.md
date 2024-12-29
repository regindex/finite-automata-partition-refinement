# finite-automata-partition-refinement

### Description

<strong> finite-automata-partition-refinement </strong> is a tool implementing the partition refinement algorithm which offers four main functionalities: 1) Sorting (Pseudo)-Wheeler automata, 2) Computing the two automata recognizing infima and suprema strings, 3) Compute the co-lexicographic intervals of the automaton states, 4) Computing the chain decomposition of arbitrary automata. 

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* The cmake build system, version \geq 3.10.
* A Linux or MacOS 64-bit operating system.

### Download

To clone the repository and install the dependencies, run:

```console
git clone http://github.com/regindex/finite_automata-partition-refinement
cd finite-automata-partition-refinement
git submodule update --init --recursive
```

### Compile

You can compile using cmake and make:

```console
mkdir build
cd build
cmake ..
make
```

### Run on example data

```console
// Take in input an automaton stored in graphML format and compute chain decomposition
./build/index32.x data/rand-NFA.graphml data/rand-NFA-sorted
// Use this bin in case you want some verbosity (only for very small examples)
./build/index32-verb.x data/rand-NFA.graphml data/rand-NFA-sorted
```

### References and Citations 

[1] Ruben Becker, Manuel Cáceres, Davide Cenzato, Sung-Hwan Kim, Bojana Kodric, Francisco Olivares, Nicola Prezza: Sorting Finite Automata via Partition Refinement. ESA 2023: 15:1-15:15 ([go to the paper](https://drops.dagstuhl.de/entities/document/10.4230/LIPIcs.ESA.2023.15))

[2] Ruben Becker, Manuel Cáceres, Davide Cenzato, Sung-Hwan Kim, Bojana Kodric, Francisco Olivares, Nicola Prezza: Sorting Finite Automata via Partition Refinement. CoRR abs/2305.05129 (2023) ([go to the paper](https://arxiv.org/abs/2305.05129))

Please, if you use this tool in an academic setting, cite the following paper:

    @inproceedings{BeckerCCKKOP23,
      author       = {Ruben Becker and
                      Manuel C{\'{a}}ceres and
                      Davide Cenzato and
                      Sung{-}Hwan Kim and
                      Bojana Kodric and
                      Francisco Olivares and
                      Nicola Prezza},
      title        = {Sorting Finite Automata via Partition Refinement},
      booktitle    = {In Proceedings of the 31st Annual European Symposium on Algorithms, {ESA} 2023},
      series       = {LIPIcs},
      volume       = {274},
      pages        = {15:1--15:15},
      year         = {2023},
      doi          = {10.4230/LIPICS.ESA.2023.15}
    }

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208.