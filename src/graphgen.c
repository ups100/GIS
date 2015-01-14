#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "io.h"

int generateGraph(int ***graph, int nbv, int nbe);
int saveGraph(int **graph, int nbv, int nbe);
void showUsage ()
{
    fprintf(stderr,
            "Usage:\n"
            " gisgraphgen -h | -help | --help : print help\n"
            "Options:\n"
            " -v [V] -e [E] : generate graph with V vertices and E edges\n"
            );
}

int main(int argc, char **argv)
{
    int ret = 0; /// returned value

    int **graph; /// graph in a whole
    int nbv; /* vertices */
    int nbe; /* edges */

    if ((argc < 3) && (argc > 0 || strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "-help") == 0 || strcmp (argv[1], "--help") == 0)) {
        showUsage ();
        return 0;
    }

    if (argc > 4 && (strcmp(argv[1], "-v") == 0 && strcmp(argv[3], "-e") == 0)) {
        nbv = atoi(argv[2]);
        nbe = atoi(argv[4]);

        ret = generateGraph(&graph, nbv, nbe);
        //        print_graph(graph);
        if (ret >= 0) {
            ret = saveGraph(graph, nbv, nbe);
            free_graph(graph);
        }

    }
    else {
        showUsage();
        return 0;
    }

    return ret;
}

static int get_vector_size(int *vec)
{
    int i = 0;

    for (i=0; vec[i] >= 0; ++i);

    return i;
}

int generateGraph(int ***graph, int nbv, int nbe)
{
    if (nbe > (nbv*(nbv-1))/2 ) {
        printf("Number of edges (%d) is too big (verticles %d)!\n", nbe, nbv);
        return -1;
    }

    printf("Generating graph %d vertices %d edges.", nbv, nbe);

    int ret = 0;
    int **g;
    int i;
    int j;

    int nbgen_e = 0;
    int n_rand;
    int temp_size;

    g = calloc(nbv + 1, sizeof(*g));
    if (!g) {
        ret = -ENOMEM;
        printf("calloc failed\n");
        return ret;
    }

    for (i = 0; i < nbv; ++i) {
        g[i] = calloc(nbe + 1, sizeof(*(g[i])));
        if (!g[i]) {
            ret = -ENOMEM;
            printf("calloc failed\n");
            return ret;
        }
        g[i][0] = -1;
    }

    srand(time(NULL));

    while (nbe) {
        for (i = 0; i < nbv; ++i) {
            for (j = 0; j < nbv; ++j) {
                n_rand = rand() % 2;
                if (n_rand == 1) {
                    temp_size = get_vector_size(g[j]);
                    g[j][temp_size] = rand() % nbv;
                    g[j][++temp_size] = -1;
                    ++nbgen_e;

                    if (nbgen_e >= nbe)
                        goto end;
                }
            }
        }
    }

end:

    *graph = g;

    return ret;
}

int saveGraph(int **graph, int nbv, int nbe)
{
    int **g;
    int *v;
    int i = 0;
    int ret = 0;
    FILE *fp;
    char dirty_buf[1024];

    char c_nbv[16];
    char c_nbe[16];

    snprintf(c_nbv, sizeof(c_nbv), "%d", nbv);
    snprintf(c_nbe, sizeof(c_nbe), "%d", nbe);
    snprintf(dirty_buf, sizeof(dirty_buf), "%s_%s_%s.txt", "graph", c_nbv, c_nbe);

    fp=fopen(dirty_buf,"w");
    if(fp==NULL){
        perror("Error opening file");
        return 1;
    }

    printf(" Save to \"%s\" file.\n", dirty_buf);

    for (g = graph; *g; ++g)
        ++i;

    snprintf(dirty_buf, sizeof(dirty_buf), "%d", i);
    fputs(dirty_buf, fp);
    fputc('\n', fp);

    for (g = graph; *g; ++g) {
        for (v = *g; *v >= 0; ++v) {
            snprintf(dirty_buf, sizeof(dirty_buf), "%d", *v);
            fputs(dirty_buf, fp);
            fputc(' ', fp);
        }
        fputc('\n', fp);
    }

    return ret;
}
