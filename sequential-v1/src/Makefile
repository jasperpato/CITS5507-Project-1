IDIR = ../include
CC = cc
CFLAGS = -I$(IDIR) -std=c11 -Wall -Werror

ODIR = obj
LDIR = ../lib

LIBS =

_DEPS = bond.h queue.h site.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bond.o percolate.o queue.o site.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

percolate: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 