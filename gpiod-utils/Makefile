FLAGS =	-Wall -Wextra -g -lgpiod

gpiod-utils.o: src/gpiod-utils.c include/gpiod-utils.h
	gcc -c src/gpiod-utils.c

.PHONY: clean

clean: 
	rm gpiod-utils.o
