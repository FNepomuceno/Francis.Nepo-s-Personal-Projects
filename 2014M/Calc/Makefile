.PHONY: all clean numlines debug todo
drt := source/
dcm := documentation/
src := .c
dst := .o
cod := $(wildcard $(drt)*$(src))
obj := $(cod:$(src)=$(dst))
ndr := $(notdir $(cod))
org := $(ndr:$(src)=$(dst))
name := calc

all: $(obj)
	@gcc $(org) -Wall -Werror -o $(name)
	@rm -f *.o
	@objdump -D $(name) > $(dcm)$(name).txt
%.o:
	@gcc -c $*.c
clean:
	@rm -f $(name)
	@rm -f *.o
	@rm -f debug.txt
	@rm -f $(dcm)$(name).txt
	@reset; ls --color=auto
numlines:
	@cat source/header/*.h source/*.c | wc -l
todo:
	@reset; ls --color=auto
	@cat source/header/*.h source/*.c | grep 'TODO' --color=AUTO
debug:
	@make clean; make > debug.txt 2>&1; valgrind --leak-check=full ./calc >> debug.txt 2>&1; clear; cat debug.txt | less
