FLAGS =	-Wall -Wextra -g

raspin: raspin.o raspin-manager.o
	gcc -o raspin src/raspin.c src/raspin-manager.c ${FLAGS}

raspin.o: src/raspin.c include/raspin-manager.h
	gcc -c src/raspin.c

raspin-manager.o: src/raspin-manager.c include/raspin-manager.h
	gcc -c src/raspin-manager.c

.PHONY: clean

clean: 
	rm raspin raspin.o raspin-manager.o
