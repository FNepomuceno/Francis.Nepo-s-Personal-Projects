.PHONY: all clean
src := .c
dst := .o
cod := $(wildcard *$(src))
obj := $(cod:$(src)=$(dst))

all: $(obj)
	@gcc $(obj) -o calc
%.o:
	@gcc -c $*.c
clean:
	@rm -f calc
	@rm -f *.o
