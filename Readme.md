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
```
./prodcon --help

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
need or demand to be completely compliant with the statement
requirements.

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
algorithms. Both versions are recursive and use the insertion sort for
sorting small vectors. Although insertion sort is O(n^2), it is very
simple, so its constant costs are cheap (parameter passing, no
recursion, etc) and tends to O(n) for lightly unsorted (or ordered
ones) partitions, which usually happens as the quicksort progresses
and goes reducing the partitions.

Because the requested programs are multi-thread, I have taken care in
the stack consumption, concretely the maximum number of nested
recursion calls. This is not a problem with mergesort because always
the partitions are equitable (half of the vector). Therefore, the
maximum number of nested recursion calls is O(log n), which is
manageable even for relatively stack sizes. 

In the case of quicksort, a degenerated case could cause partitions
tend to O(n) and so on, what could cause O(n) nested calls overflowing
even a big stack. In order to avoid that, the quicksort inspects the
partition sizes and choices to recursively sort the smallest partition
first. In this way the worst case is O(log n). In addition, the pivot
is selected between the median of extremes plus the element in the
middle, what mitigates, but does not avoid, the risk of getting bad
partitions. 

### Possible Improvements

#### Sorting Methods

A more elaborate o tuned sorting algorithm could be used. Probably an
introsort version that combines quicksort and heapsort according the
depth of recursive calls and takes more care of pivot selection. Also,
the partition method could be adapted for better profiting of hardware
cache (it exists versions that perform the partition sequentially from
left to right, by contrast with my version that traverse both sides
and eventually could cause more cache misses).

#### Several files

In this version, the sorted sequences are put in final file through a
`OutputFile` object (see `include/outputfile.H`). This class uses
a `mutex` in order to assure mutual exclusion when a thread is writing
a vector. However, this mutex could contend consumer threads that have
already completed the sorting and want to write the result.

A possible improvement, not explored, could consist in manage
separated files, one file by thread. In this way, no thread would wait
for writing. It would just write without any contention. As each
thread completes, it concatenate its file to a resulting file.

#### Inlining and separated compilation units

For time reasons, I preferred to put my helpers classes and function
in header files. Of course, an alternative, especially if 
encapsulation and information hiding is required, consist in handle
separated compilation units. In this case:

1. The headers files would only contain the methods declarations, or
   even a more sophisticated approach (pimpl idiom for example), and

2. A `src` directory containing the source implementation would be
   used and every unit would be compiled. 

