#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "io.h" /* for free_graph */
#include "algorithm.h"

/*
  Generates a table which match each node with its scc
*/
static int *generate_v_scc_match(int **g, int size, int **sccs)
{
    int *v_to_scc;
    int i, j;

    v_to_scc = calloc(size + 1, sizeof(*v_to_scc));
    if (!v_to_scc)
        goto out;
    v_to_scc[size] = -1;

    for (i = 0; sccs[i]; ++i) {
        for (j = 0; sccs[i][j] >= 0; ++j) {
            v_to_scc[sccs[i][j]] = i;
        }
    }
out:
    return v_to_scc;
}

/*
  Generates metagraph based on sccs
*/
static int **generate_meta_graph(int **g, int size, int **sccs, int n_sccs, int *v_to_scc)
{
    int **mg;
    int i, j, k;
    int n;

    mg = calloc(n_sccs + 1, sizeof(*mg));
    if (!mg)
        goto out;

    /* allocate the memory for our metagraph */
    for (i = 0; sccs[i]; ++i) {
        n = 0;
        for (j = 0; sccs[i][j] >= 0; ++j) {
            int node = sccs[i][j];
            for (k = 0; g[node][k] >= 0; ++k)
                if (v_to_scc[node] != v_to_scc[g[node][k]])
                    ++n;
        }

        mg[i] = calloc(n + 1, sizeof(**mg));
        if (!mg[i])
            goto free_mg;
    }

    /* generate edges */
    for (i = 0; sccs[i]; ++i) {
        n = 0;
        for (j = 0; sccs[i][j] >= 0; ++j) {
            int node = sccs[i][j];
            for (k = 0; g[node][k] >= 0; ++k) {
                if (v_to_scc[node] != v_to_scc[g[node][k]]) {
                    mg[i][n++] = v_to_scc[g[node][k]];
                }
            }
        }
        mg[i][n] = -1;
    }

    return mg;

free_mg:
    free_graph(mg);
out:
    return NULL;
}

static int *wcc = NULL;
static int **wccs = NULL;
static int counter = 0;
static int pos = 0;
static int **graph = NULL;

static int get_vector_size(int *vec)
{
    int i = 0;

    for (i=0; vec[i] >= 0; ++i);

    return i;
}

#include "io.h"

static void f_longest(int n)
{
    int i;
    int j;
    int k;
    int l;
    int m;

    wcc[pos++] = n;

    /* if we have some edges we run DFS */
    for (i = 0; graph[n][i] >= 0; ++i) {
        f_longest(graph[n][i]);
    }

    /* That's end of our path */
    if (i == 0) {

        int *cand_new_wcc = calloc(pos + 1, sizeof(*cand_new_wcc));;
        memcpy(cand_new_wcc, wcc, pos*sizeof(*wcc));

        int is_new_in_wccs = 1;
        for (j = 0; j < counter; ++j) {  /* by all wccs */
            /* pos is equivalent of get_vector_size(wcc) but
           doesn't need to be -1 terminated */
            int vector_size = get_vector_size(wccs[j]);
            if (vector_size == pos) {
                if (memcmp(wcc, wccs[j], pos*sizeof(*wcc)) == 0) {
                    /* is the same */
                    is_new_in_wccs = 0;

                    break; /* end search */
                }
            }
            else if (vector_size < pos) { /* new greater */
                int delete_others = 0;
                for (m = j; m < counter; ++m) { /* by other wccs */
                    if (get_vector_size(wccs[m]) >= pos) {
                        continue;
                    }

                    int break_m_loop = 0;
                    int is_cand_cont_wcc = 1;
                    for (k = 0; k < vector_size; ++k) { /* by all elements j wcc */
                        int temp_wcc_v = wccs[m][k];

                        int is_new_v_in_wcc = 0;
                        for (l = 0; l  < pos; ++l) { /* by all elements wcc candidat */
                            if(cand_new_wcc[l] == temp_wcc_v) {
                                is_new_v_in_wcc = 1;
                                break; /* go check next element j wcc */
                            }
                        }
                        if (is_new_v_in_wcc == 0) {
                            is_cand_cont_wcc = 0;

                            break_m_loop = 1; /* j wcc candidat not contains wcc */
                            break;
                        }
                    }
                    if (break_m_loop) break;
                    if (is_cand_cont_wcc == 1) {
                        if (delete_others == 1) { /* delete wcc */
                            --counter;
                            memcpy(wccs[m], wccs[counter], get_vector_size(wccs[counter])*sizeof(*wccs[counter]));

                            wccs[counter][0] = -1;
                            wccs[counter] = NULL;
                        }
                        else if (delete_others == 0) { /* swap wcc */
                            is_new_in_wccs = 0;
                            cand_new_wcc[pos++] = -1;
                            wccs[m] = realloc(wccs[m], pos*sizeof(*cand_new_wcc));
                            memcpy(wccs[m], cand_new_wcc, pos*sizeof(*cand_new_wcc));

                            --pos;
                            delete_others = 1;

                        }
                    }
                }
            }

            else if (vector_size > pos) { /* new smaller */
                int is_wcc_cont_cand = 1;
                for (k = 0; k < pos; ++k) { /* by all elements wcc candidat */
                    int temp_wcc_v =  cand_new_wcc[k];

                    int is_new_v_in_wcc = 0;
                    for (l = 0; l < vector_size; ++l) { /* by all elements j wcc */
                        if(wccs[j][l] == temp_wcc_v) {
                            is_new_v_in_wcc = 1;
                            break; /* go check next element wcc candidat */
                        }
                    }

                    if (is_new_v_in_wcc == 0) {
                        is_wcc_cont_cand = 0;
                        break; /* go check next elemnt wcc candidat */
                    }
                }

                if (is_wcc_cont_cand == 1) {
                    is_new_in_wccs = 0;
                    break; /* go out */
                }
            }
        }

        if (is_new_in_wccs) {
            wcc[pos++] = -1;
            wccs[counter] = calloc(pos, sizeof(*wcc));
            assert(wccs[counter]);

            memcpy(wccs[counter], wcc, pos*sizeof(*wcc));
            ++counter;
            --pos;
        }
	free(cand_new_wcc);
    }

    --pos;

    return;
}

static int **find_longest(int **mg, int size)
{
    int **tmg;
    int **ret;
    int i;

    /* Initialize global variables */
    counter = 0;
    graph = mg;

    /* There will be no more wcc than scc */
    wccs = calloc(5+size + 1, sizeof(*wccs));
    if (!wccs)
        goto out;

    /* wcc is used as dirty buffer for current wcc construction */
    wcc = calloc(size + 1, sizeof(*wcc));
    if (!wcc)
        goto free_wccs;

    /* generate transposed graph to easily check if node has incomming edges */
    tmg = transpose(mg, size);
    if (!tmg)
        goto free_wcc;

    for (i = 0; i < size; ++i) {

        /* if we don't have incomming edges */
        if (tmg[i][0] < 0) {
            pos = 0;
            f_longest(i);
        }
    }

    free_graph(tmg);
    /* cleanup global variables */
    ret = wccs;
    wccs = NULL;
    free(wcc);
    wcc = NULL;
    graph = NULL;

    return ret;

free_wcc:
    free(wcc);
free_wccs:
    free(wccs);
out:
    return NULL;
}

static int check_size(int **g)
{
    int i;

    for (i = 0; g[i]; ++i);

    return i;
}

/*
  Generates wcc from paths in metagraph
*/
static int **paths_to_wccs(int **paths, int **sccs)
{
    int n_paths;
    int **ret;
    int i, j, k, n, needed;

    n_paths = check_size(paths);

    ret = calloc(n_paths + 1, sizeof(*ret));
    if (!ret)
        goto out;

    /* allocate the memory */
    for (i = 0; i < n_paths; ++i) {

        needed = 0;
        for (j = 0; paths[i][j] >= 0; ++j) {
            for (k = 0; sccs[paths[i][j]][k] >= 0; ++k) {
                ++needed;
            }
        }

        ret[i] = calloc(needed + 1, sizeof(**ret));
        if (!ret[i])
            goto free_ret;
    }

    for (i = 0; i < n_paths; ++i) {
        n = 0;
        for (j = 0; paths[i][j] >= 0; ++j) {
            for (k = 0; sccs[paths[i][j]][k] >= 0; ++k) {
                ret[i][n++] = sccs[paths[i][j]][k];
            }
        }
        ret[i][n] = -1;
    }
out:
    return ret;

free_ret:
    free_graph(ret);
    return NULL;
}

int **generate_wccs(int **g, int size)
{

    int **sccs = NULL; /// strongly connected components
    int **wccs = NULL;
    int **paths = NULL;
    int **mg = NULL;
    int *v_to_scc = NULL;
    int n_sccs;

    sccs = kosoraju(g, size);

    if (!sccs)
        goto out;

    v_to_scc = generate_v_scc_match(graph, size, sccs);
    if (!v_to_scc)
        goto free_sccs;

    n_sccs = check_size(sccs);

    mg = generate_meta_graph(g, size, sccs, n_sccs, v_to_scc);
    if (!mg)
        goto free_v_scc;

    paths = find_longest(mg, n_sccs);
    if (!paths)
        goto free_mg;

    wccs = paths_to_wccs(paths, sccs);

    free_graph(paths);
free_mg:
    free_graph(mg);
free_v_scc:
    free(v_to_scc);
free_sccs:
    free_graph(sccs);
out:

    return wccs;
}


