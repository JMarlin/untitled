include ../Makefile.inc

SRC=$(wildcard *.c)
OBJ=$(addprefix ../build/,$(notdir $(SRC:.c=.o)))

all: $(OBJ)

../build/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
 
