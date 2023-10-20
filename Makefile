all: build

build:
	gcc -Wall -Werror launcher.c read.c write.c transmit.c receive.c list.c -o s-talk -lpthread
	
clean:
	rm -f s-talk