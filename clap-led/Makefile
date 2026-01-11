FLAGS =	-Wall -Wextra -g -lgpiod

clap-led: clap-led.o gpiod-utils.o
	gcc -o clap-led src/clap-led.c src/gpiod-utils.c ${FLAGS}

clap-led.o: src/clap-led.c include/gpiod-utils.h
	gcc -c src/clap-led.c

gpiod-utils.o: src/gpiod-utils.c include/gpiod-utils.h
	gcc -c src/gpiod-utils.c

.PHONY: clean

clean: 
	rm clap-led clap-led.o gpiod-utils.o