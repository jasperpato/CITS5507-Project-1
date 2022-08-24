IDIR = ../include
# CC = /usr/local/bin/gcc-12
CFLAGS = -I$(IDIR) -std=c11 -Wall -Werror -fopenmp -lm

ODIR = obj
LDIR = ../lib

LIBS = 

_DEPS = constant.h bond.h cluster.h site.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bond.o percolate.o cluster.o site.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: percolate

percolate: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

util/params: util/params.c
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 