# Prodcon

This project implements a producer/consumer file reading pattern in
which a text file whose lines contain numbers are sorted and written
in an output file.

## Building

### Dependencies

- `cmake`: available in almost linux distro
- `tclap` command parser library: also available in the most part of
  linux distros. On Ubuntu distros the installation can be done thus:
  
      sudo apt-get install libtclap-dev

- A C++ compiler able to compile C++-17. That it should not be a
  problem in modern linux distros. 

### Compilation

Assuming that every dependency is met, perform:

    cmake CMakeLists.txt
    make

## Programs included

### `prodcon`

This is the main executable and it is intended as deliverable. It uses
`tclap` in order to parse the command line. Its main advantage is that
you can set the number of worker threads, use more sorting methods and
perform an optional correctness test.

This is the output of help option:
```./prodcon --help

USAGE: 

   ./prodcon  [-S <insertion|merge|std|quick>] [-t] [-n <number of
              threads>] -o <output file name> -i <input file name> [--]
              [--version] [-h]


Where: 

   -S <insertion|merge|std|quick>,  --sort-type <insertion|merge|std|quick>
     sorting method

   -t,  --test
     run correctness test

   -n <number of threads>,  --num_threads <number of threads>
     number of threads

   -o <output file name>,  --output <output file name>
     (required)  output file name

   -i <input file name>,  --input <input file name>
     (required)  input file name

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.


   prodcon
```

### `exam`

This is an equivalent version of `prodcon` that does not use `tclap`
library. Consequently, it only exports two sorting methods: mergesort
and quicksort. Also, it does not export the optional correctness test.

Use this version if you have problems for installing `tclap` or if you
need to be completely compliant with the statement requirements.

Its syntax is

    ./exam input-file output-file merge|quick
    
### `gen-input`

This is a generator of pseudo random samples so that you can perform
several tests.

In order to generate a small input file:

    ./gen-input -n 10 -m 10 -s 13 -L 0 -H 100 > input-10-10-13.txt
    
And a big one:

    ./gen-input -n 100000 -m 10000 -s 19 -L 0 -H 10000 > input-100000-10000-19.txt

The flags are:

- `-n`: number of lines.
- `-m`: number of samples by line.
- `-s`: seed of random number generator.
- `-L`: lowest sample value.
- `-H`: highest sample value.

## Notes for the examiner 

### About the selection of sorting methods

I have chose quicksort and mergesort as my proposed sorting
algorithms. Both version are recursive and uses the insertion sort for
sorting small vector. Although insertion sort is O(n^2), it is very
simple, so its constant costs are cheap (parameter passing, no
recursion, etc) and tends to O(n) for lightly unsorted partitions,
which usually happens as the quicksort goes reducing the partitions.

Becau

### Possible Improvements

#### Sorting Methods

#### Linked list

#### Several files

#### Inlining and separated compilation units

### Pending things that I would have wanted to do

