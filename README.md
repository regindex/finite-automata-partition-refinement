# finite-automata-partition-refinement

### Description

<strong> finite-automata-partition-refinement </strong> is a tool implementing the partition refinement algorithm, and offers three main functionalities: 1) Sorting (Pseudo)-Wheeler NFAs, 2) Pruning an input automaton in to two automata recognizing infima and suprema strings, 3) Compute the co-lexicographic intervals of the automaton states. 

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

This tool can take in input an automaton in .dot format; the source state must have the smallest id (see below for an example),
```
strict digraph {
  S1 -> S2 [ label = 65 ];
  S3 -> S2 [ label = 65 ];
  S6 -> S2 [ label = 65 ];
  S10 -> S2 [ label = 65 ];
  S12 -> S2 [ label = 65 ];
  S13 -> S2 [ label = 65 ];
  S14 -> S3 [ label = 65 ];
  S16 -> S4 [ label = 65 ];
  S3 -> S5 [ label = 67 ];
  ...
}
```
The tool can also accept an intermediate format: it consists of one edge per line (origin edge-label destination) plus the first line containing numbers of states, number of edges, source state, and number of accepting states (see below for an example).
```
6 5 2 1
2 3 $
3 5 A
5 0 A
5 4 C
0 1 A
```

### Usage

```
usage: partition_refinement.py [-h] [-outpath OUTPATH] [--sort] [--prune] [--intervals] [--compact] [--dot] [--source SOURCE] [--idbase1] [--verbose] input

Tool to sort and prune finite automata using the partition refinement algorithm.

positional arguments:
  input             automaton file path

optional arguments:
  -h, --help        show this help message and exit
  -outpath OUTPATH  output file basename (def. filename.part)
  --sort            sort a (Pseudo-)Wheeler automaton using the partition refinement algorithm (def. False)
  --prune           compute pruned automata recognizing infima and suprema strings (def. False)
  --intervals       compute the colex. intervals of the input automaton (def. False)
  --compact         write the output automata in compact format (def. False)
  --dot             take in input a dot file (def. False)
  --source SOURCE   Only for .dot inputs, define the source state of the input automaton (def. 0)
  --idbase1         activate if state ids start from 1 rather than from 0 (def. False)
  --verbose         verbose mode on (def. False)
```
Note that all input automata are supposed to be input consistent, i.e., it does not exist a node reached by two edges labelled with two different characters.

### Run on example data

```console
// Take in input a (Pseudo)-Wheeler automata and run algorithm (1)
python3 partition_refinement.py --dot --idbase1 --sort data/randWG.dot
// Take in input a finite automata (it must be input consistent) and run algorithm (2) and (3)
python3 partition_refinement.py --prune --compact data/randWG.inter
python3 partition_refinement.py --intervals data/randWG.inter
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