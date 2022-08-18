# Sequential v1

To run:

```console
cd sequential-v1
cc -o percolate percolate.c site.c bond.c queue.c
./percolate [-b | -s] [[-f FILENAME] | [LATTICE_SIZE PROBABILITY]]
```

Site file example:

```
X X O X
O X O O
X X X O
X X X O
```

Bond file example:

```
 | |
-O-O O
 | |
 O O-O
 |
-O O-O
```
