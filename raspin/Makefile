FLAGS =	-Wall -Wextra -g -lgpiod

raspin: raspin.o raspin-manager.o gpiod-utils.o
	gcc -o raspin src/raspin.c src/raspin-manager.c src/gpiod-utils.c ${FLAGS}

raspin.o: src/raspin.c include/raspin-manager.h include/gpiod-utils.h
	gcc -c src/raspin.c

raspin-manager.o: src/raspin-manager.c include/raspin-manager.h include/gpiod-utils.h
	gcc -c src/raspin-manager.c

gpiod-utils.o: src/gpiod-utils.c include/gpiod-utils.h
	gcc -c src/gpiod-utils.c

.PHONY: clean

clean: 
	rm raspin raspin.o raspin-manager.o gpiod-utils.o
