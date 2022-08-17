# Sequential v1

To run:

```console
cd sequential-v1
cc -o seq-v1 percolate.c site.c bond.c queue.c
./seq-v1 -b | -s [-f FILENAME] [LATTICE_SIZE PROBABILITY]
```

Site file contents example:
X X O X  
O X O O  
X X X O  
X X X O
