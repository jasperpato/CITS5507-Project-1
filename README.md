# CITS5507 HPC PROJECT 1

In this project we simulate percolation within a variable-length square lattice, and report the key details of the percolation: number of clusters, maximum cluster size, whether any cluster spans an entire row or column of the lattice, and time taken for the simulation.

### Usage

To run:

```console
cd src
make
./percolate [-s | -b] [-r SEED] [-p RESULTS_FILENAME] [[-f LATTICE_FILENAME] | [N PROBABILITY]] [N_THREADS]
```

- [-s | -b] site or bond percolation, default site
- [-v] silence printing
- [-r SEED] number to seed srand, default time(NULL)
- [-p RESULTS_FILENAME] file to append the results of the percolation
- [-f LATTICE_FILENAME] file to scan lattice from
- [N PROBABILITY] size of lattice and probability of site occupation or bond
- [N_THREADS] number of threads to utilise

Examples:

- ./percolate -s 500 0.4
- ./percolate -s 500 0.4 2

- ./percolate -b -f ../lattice/bond20.txt 20
- ./percolate -b -f ../lattice/bond20.txt 20 4

- ./percolate -r 999 10 0.4
- ./percolate -r 999 10 0.4 2

- ./percolate -v -p ../test/results.csv 100 0.1

To generate results:

```console
cd test
make
./driver
```

To graph results:

```console
cd test
python3 results.py [-n N | -p P] [--fname RESULTS_FILE]
```

- [-n N] keep n constant at N and graph all N_THREADS
- [-p P] keep p constant at P and graph all N_THREADS
- [--fname RESULTS_FILE] results file name

Examples:

- python3 results.py -n 2500
- python3 results.py -p 0.4
