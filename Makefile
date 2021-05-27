CC = gcc
CFLAGS = -Wall -c -o $@
all: 3dgraph

3dgraph: 3d.o list.o graph.o main.o
	gcc $^ -lSDL2 -lm -o $@

3d.o: 3d.c
	gcc $^ $(CFLAGS)

list.o: list.c
	gcc $^ $(CFLAGS)

graph.o: graph.c
	gcc $^ $(CFLAGS)

main.o: main.c
	gcc $^  $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f main
