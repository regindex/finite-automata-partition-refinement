# WNFA-partition-refinement

### Description

This is a tool to sort Wheeler-NFA using the partition refinement algorithm.

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

The tool takes in input a graph in .dot format (at the moment it is tested only with WGT like graphs).

### Usage

```
usage: partition_refinement.py [-h] [-o O] [--idbase1] [--check] [--verbose]
                               input

Tool to sort Wheeler NFAs using partition refinement algorithm.

positional arguments:
  input       input dot file name

optional arguments:
  -h, --help  show this help message and exit
  -o O        output files basename (def. tmp.part)
  --idbase1   activate if root nodes is labelled with S1 (def. False)
  --check     check output correctness (def. False)
  --verbose   verbose mode on (def. False)
```

### Run on example data

```console
python3 -o out.dot --idbase1 randWG.dot 
```

# External resources

* [malloc_count](https://github.com/bingmann/malloc_count)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208