
all: main

main: main.o graph.o 3d.o
	gcc main.o graph.o 3d.o -lSDL2 -o main

main.o: main.c
	gcc main.c -c -o main.o -Wall

graph.o: graph.c
	gcc graph.c -c -o graph.o -Wall

3d.o: 3d.c
	gcc 3d.c -c -o 3d.o -Wall
