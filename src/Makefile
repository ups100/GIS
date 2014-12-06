all: gis

gis: gis.o io.o wcc.o kosaraju.o
	gcc gis.o io.o wcc.o kosaraju.o -o gis

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf *.o gis

