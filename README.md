# finite-automata-partition-refinement

### Description

<strong> finite-automata-partition-refinement </strong> is a tool implementing the partition refinement algorithm, and offers three main functionalities: 1) Sorting (Pseudo)-Wheeler automata, 2) Pruning an (input-consistent) automaton in to two automata recognizing infima and suprema strings, 3) Compute the co-lexicographic intervals of an (input-consistent) automaton states. 

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* The cmake build system, version \geq 3.10.
* A Linux 64-bit operating system.

### Download

To clone the repository, run:

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

### Input

This tool can take in input an input-consistent automaton in .dot format; the source state must have the smallest id (see below for an example),
```
strict digraph {
  S1 -> S2 [ label = 84 ];
  S2 -> S3 [ label = 65 ];
  S3 -> S4 [ label = 84 ];
  S4 -> S5 [ label = 65 ];
  S5 -> S2 [ label = 84 ];
  S5 -> S6 [ label = 67 ];
  S6 -> S6 [ label = 67 ];
  S6 -> S7 [ label = 65 ];
  S7 -> S8 [ label = 67 ];
  S8 -> S9 [ label = 84 ];
  S9 -> S7 [ label = 65 ];
  S9 -> S4 [ label = 84 ];
}
```
The tool can also accept an intermediate format: it consists of one edge per line (origin edge-label destination) plus the first line containing numbers of states, number of edges, source state, and number of accepting states (see below for an example).
```
9 12 0 1
0 T 1
1 A 2
2 T 3
3 A 4
4 T 1
4 C 5
5 C 5
5 A 6
6 C 7
7 T 8
8 A 6
8 T 3
8
```

### Usage

```
usage: partition_refinement.py [-h] [-outpath OUTPATH] [--sort] [--prune] [--intervals] [--compact] [--intermediate] [--keep] [--verbose] input

Tool to sort and prune finite automata using the partition refinement algorithm.

positional arguments:
  input             automaton file path

options:
  -h, --help        show this help message and exit
  -outpath OUTPATH  output file basename (def. filename.part)
  --sort            sort a (Pseudo-)Wheeler automaton using the partition refinement algorithm (def. False)
  --prune           compute pruned (infsup-automaton) automaton recognizing infima and suprema strings (def. False)
  --intervals       compute the colex. intervals of the input automaton (def. False)
  --compact         write the output automata in compact format (def. False)
  --intermediate    take in input an intermediate file instead of a .dot file (def. False)
  --keep            keep intermediate files (def. False)
  --verbose         verbose mode on (def. False)
```
Note that all input automata are supposed to be input consistent, i.e., it must hold that each automaton state is reached by edges labeled with the same character.

### Run on example data

```console
// Take in input a (Pseudo)-Wheeler automata and run algorithm (1)
python3 partition_refinement.py --sort data/randWG.dot
// Take in input a finite automata (input consistent) and run algorithm (2) and (3)
python3 partition_refinement.py --prune --compact --intermediate data/randWG.inter
python3 partition_refinement.py --intervals --intermediate data/randWG.inter
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

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208