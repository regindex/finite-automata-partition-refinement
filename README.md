# finite-automata-partition-refinement

### Description

This is a tool to sort Wheeler NFAs and prune input consistent automata using the partition refinement algorithm.

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

The tool takes in input an automaton either in .dot format, the initial state must the one with the smallest id (see below for an example),
```
strict digraph {
  S1 -> S2 [ label = 0 ];
  S3 -> S2 [ label = 0 ];
  S6 -> S2 [ label = 0 ];
  S10 -> S2 [ label = 0 ];
  S12 -> S2 [ label = 0 ];
  S13 -> S2 [ label = 0 ];
  S14 -> S3 [ label = 0 ];
  S16 -> S4 [ label = 0 ];
  S3 -> S5 [ label = 1 ];
  ...
}
```
or using an intermediate format, one edge per line plus the last line containing numbers of states, number of edgesm source abd number of accepting states  (see below for an example).
```
2 3 $
3 5 A
5 0 A
5 4 C
0 1 A
6 5 2 1
```

### Usage

```
usage: partition_refinement.py [-h] [-o O] [--pruning] [--ascii]
                               [--source SOURCE] [--idbase1] [--intermediate]
                               [--verbose]
                               input

Tool to sort and prune finite automata using partition refinement algorithm.

positional arguments:
  input            input automaton file name

optional arguments:
  -h, --help       show this help message and exit
  -o O             output file basename (def. filename.part)
  --pruning        compute pruned automaton recognizing infimum strings (def. False)
  --ascii          use ascii alphabet to encode edge labels rather than DNA alphabet (def. False)
  --source SOURCE  Only for .dot inputs, define the source state (def. 0)
  --idbase1        activate if state ids start from 1 rather than from 0 (def. False)
  --intermediate   take in input an intermediate file (def. False)
  --verbose        verbose mode on (def. False)
```

### Run on example data

```console
// Run the partition refinement algorithm on a random Wheeler NFA
python3 partition_refinement.py --idbase1 --source 0 --verbose data/randWG.dot
python3 partition_refinement.py --ascii --intermediate --verbose data/randWG.inter
// Run pruning algorithm on a random DFA
python3 partition_refinement.py  --ascii --intermediate --pruning --verbose data/DFA.inter
```

### External resources

* [malloc_count](https://github.com/bingmann/malloc_count)

### Citation 

Please, if you use this tool in an academic setting, cite the following paper:

    @article{BeckerCCKKOP23,
      author    = {Ruben Becker and
                   Manuel C{\'{a}}ceres and
                   Davide Cenzato and
                   Sung{-}Hwan Kim and
                   Bojana Kodric and
                   Francisco Olivares and
                   Nicola Prezza},
      title     = {Sorting Finite Automata via Partition Refinement},
      journal   = {CoRR},
      volume    = {abs/2305.05129},
      pages     = {129--142},
      year      = {2023},
      doi       = {10.48550/arXiv.2305.05129}
    }

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208