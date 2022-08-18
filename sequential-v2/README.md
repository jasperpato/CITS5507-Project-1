# Sequential v2

To run:

```console
cd sequential-v1
cc -o percolate percolate.c site.c bond.c queue.c
./percolate [-b | -s] [[-f FILENAME] | [LATTICE_SIZE PROBABILITY]]
```

Site file examples:

```
X X O X   XOX
O X O O   OOX
X X X O   XXO
X X X O
```

Bond file examples:

```
 | |        |
-O-O O   -O-O
 | |      O O
 O O-O
 |
-O O-O
```
