IDIR = ../include
CC = /usr/local/bin/gcc-12
CFLAGS = -I$(IDIR) -std=c11 -Wall -Werror

ODIR = obj
LDIR = ../lib

LIBS = 

_DEPS = bond.h cluster.h site.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bond.o percolate.o cluster.o site.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

percolate: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 