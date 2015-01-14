all: gis graphgen

gis: gis.o io.o wcc.o kosaraju.o
	gcc gis.o io.o wcc.o kosaraju.o -o gis

graphgen: graphgen.o io.o 
	gcc graphgen.o io.o -o graphgen

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf *.o gis graphgen

