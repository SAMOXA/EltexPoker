all: main.o stat.o
	gcc -g -o main main.o stat.o

main.o:
	gcc -c main.c

stat.o:
	gcc -c stat.c

clean:
	rm -f *.o main