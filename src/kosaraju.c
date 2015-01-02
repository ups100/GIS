#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "io.h" /* for free_graph */

/* We use global variables to allow our recursive functions go deeper */
static int *visited = NULL;
static int *new_order = NULL;
static int *scc = NULL;
static int **graph = NULL;
static int counter = 0;

int **transpose(int **g, int size)
{
    int **tg;
    int **tmp_g;
    int *needed;
    int i, j;

    tg = calloc(size + 1, sizeof(*g));
    if (!tg)
        goto out;

    needed = calloc(size, sizeof(*needed));
    if (!needed)
        goto free_t;

    /* count how much memory we need */
    for (i = 0; i < size; ++i)
        for (j = 0; g[i][j] >= 0; ++j)
            needed[g[i][j]]++;

    for (i = 0; i < size; ++i) {
        tg[i] = calloc(needed[i] + 1, sizeof(int));
        if (!tg[i])
            goto free_needed;
    }

    free(needed);

    /* We create an array of pointer which points
       to the same rows as in tg. We use this tmp graph
       to fill all rows, because tmp contains pointers to first
       free position in each row of tg
    */
    tmp_g = calloc(size + 1, sizeof(*g));
    if (!tmp_g)
        goto free_t_all;

    memcpy(tmp_g, tg, sizeof(*tg)*size);

    for (i = 0; i < size; ++i)
        for (j = 0; g[i][j] >= 0; ++j) {
            int out_node = g[i][j];
            *(tmp_g[out_node]) = i;
            ++(tmp_g[out_node]);
        }

    /* Apped -1 at end */
    for (i = 0; i < size; ++i)
        *(tmp_g[i]) = -1;

    free(tmp_g);
out:
    return tg;

free_needed:
    free(needed);

free_t_all:
    for (i = 0; i < size; ++i)
        free(tg[i]);

free_t:
    free(tg);
    return NULL;
}

/*
  DFS from node to determine new order
  Use global variables:
  - graph
  - visited
  - new_order
  - counter
*/
static void mark_node(int n)
{
    int i;
    visited[n] = 1;

    for (i = 0; graph[n][i] >= 0; ++i)
        if (!visited[graph[n][i]])
            mark_node(graph[n][i]);

    new_order[counter++] = n;
}

/*
  Finds the order in which we should visit nodes to find SCC
  Use global variables:
  - graph
  - counter
  - new_order
  - visited
*/
static int *mark_nodes(int **g, int size)
{
    int i;
    int *order = NULL;

    /* setup global variables */
    graph = g;
    counter = 0;
    visited = NULL;
    new_order = NULL;

    visited = calloc(size, sizeof(*visited));
    if (!visited)
        goto out;

    new_order = calloc(size, sizeof(*new_order));
    if (!new_order)
        goto cleanup;

    for (i = 0; i < size; ++i) {
        if (!visited[i])
            mark_node(i);
    }

    assert(counter == size);

    /* cleanup global variables */
cleanup:
    free(visited);
    order = new_order;
    visited = NULL;
    new_order = NULL;
out:
    return order;
}

/*
  Generates one SCC
  Use global variables:
  - graph
  - visited
  - counter
  - scc
*/
static void gen_scc(int n)
{
    int i;
    /* we encode here information if we had where to go */
    visited[n] = 2;

    counter++;
    for (i = 0; graph[n][i] >= 0; ++i)
        if (!visited[graph[n][i]]) {
            visited[n] = 1;
            gen_scc(graph[n][i]);
        }

    if (visited[n] == 1) {
        /* We had where to go so just add to scc */
        scc[--counter] = n;
    } else {
        visited[n] = 1;
        /* We are the last node so we have to allocate the memory */
        scc = calloc(counter + 1, sizeof(int));
        /* other way would just complicate */
        assert(scc);
        scc[counter] = -1;
        scc[--counter] = n;
    }
}

/*
  Generates an array of SCC
  Use global variables:
  - graph
  - visited
  - counter
  - scc
*/
static int **generate_sccs(int **g, int size, int *order)
{
    int **sccs = NULL;
    int i, j, k;

    /* setup global variables */
    graph = g;
    visited = NULL;

    visited = calloc(size, sizeof(*visited));
    if (!visited)
        goto out;

    sccs = calloc(size + 1, sizeof(*sccs));
    if (!sccs)
        goto cleanup;


    for (i = size - 1, j = 0; i >= 0; --i) {
        if (!visited[order[i]]) {
            /* We use counter to count number of verticles in scc */
            counter = 0;
            gen_scc(order[i]);
            assert(counter == 0);

            sccs[j++] = scc;
            scc = NULL;
        }
    }

cleanup:
    free(visited);
    visited = NULL;
out:
    return sccs;
}

int **kosoraju(int **g, int size)
{
    int **tg = NULL; /// transpose graph
    int *order = NULL;
    int **sccs = NULL; /// strongly connected components

    if (!g || size <=0)
        goto out;

    tg = transpose(g, size);
    if (!tg)
        goto out;

    order = mark_nodes(tg, size);
    if (!order)
        goto free_t_graph;

    sccs = generate_sccs(g, size, order);

    free(order);
free_t_graph:
    free_graph(tg);
out:
    return sccs;
}
