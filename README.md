# WNFA-partition-refinement

### Description

This is a tool to sort Wheeler NFAs using the partition refinement algorithm.

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* The cmake build system, version \geq 3.10.
* A Linux 64-bit operating system.

### Download

To clone the repository, run:

```console
git clone http://github.com/regindex/WNFA-partition-refinement
cd WNFA-partition-refinement
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
usage: partition_refinement.py [-h] [-o O] [--pruning] [--suprema] [--ascii]
                               [--source SOURCE] [--idbase1] [--intermediate]
                               [--verbose]
                               input

Tool to sort finite automata using partition refinement algorithm.

positional arguments:
  input            input automaton file name

optional arguments:
  -h, --help       show this help message and exit
  -o O             output files basename (def. filename.part)
  --pruning        run the pruning algorithm (def. False)
  --suprema        Only for --pruning, compute suprema strings pruned DFA (def. False)
  --ascii          use the ascii alphabet (def. False)
  --source SOURCE  Only for .dot inputs, define the source state (def. 0)
  --idbase1        activate if state ids start from 1 (def. False)
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
python3 partition_refinement.py  --ascii --intermediate --pruning --suprema --verbose data/DFA.inter
```

# External resources

* [malloc_count](https://github.com/bingmann/malloc_count)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208