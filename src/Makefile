IDIR = ../include
CC = gcc
CFLAGS = -I$(IDIR) -std=c11 -Wall -Werror -fopenmp -lm -Wno-unused-function -Wno-unused-variable

ODIR = obj
LDIR = ../lib

LIBS = 

_DEPS = bond.h cluster.h site.h stack.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bond.o cluster.o percolate.o site.o stack.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: percolate

percolate: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 