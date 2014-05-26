# Makefile
TAR = test
CC = gcc -O6 -Wall
L = -lm
OBJS = main.o ibary.o
SHAD = -fPIC -shared
LIB = libibary.so

all : $(TAR) lib
$(TAR): $(OBJS)
	  $(CC) -o $@ $(OBJS) $(L)
clean:
	  rm -f $(OBJS) $(LIB) $(TAR) *~ *.bak

.c.o:	  $(CC) -c $<om

lib:
	$(CC) $(SHAD) -o $(LIB) ibary.c $(L)

main.o: main.h ibary.o
isary.o: ibary.h
