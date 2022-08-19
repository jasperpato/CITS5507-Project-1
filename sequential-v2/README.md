# Sequential v1

###TODO:

- Add option to time the algorithm with minimised overhead such as excessive printing and error checking

###To run:

```console
cd sequential-v1/src
make
./percolate [-s | -b] [[-f FILENAME] | [LATTICE_SIZE PROBABILITY]]
```

###Site file example:

```
X X O X
O X O O
X X X O
X X X O
```

###Bond file example:

```
 | |
-O-O O
 | |
 O O-O
 |
-O O-O
```
