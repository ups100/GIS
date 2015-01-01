#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "io.h"
#include "algorithm.h"

void print_help()
{
    fprintf(stderr,
            "Input format:\n"
            "N_VERTICLES\n"
            "CONNECTIONS_FROM 0\n"
            "CONNECTIONS_FROM 1\n"
            "...\n"
            "CONNECTIONS_FROM N_VERTICLES -1\n"
            "Example:\n"
            "3\n"
            "1 2\n"
            "0 2\n"
            "0 1\n"
            );
}

# include <fcntl.h>

int main(int argc, char **argv)
{
    int **graph; /// graph in a whole
    int **wccs;
    int ret = -EINVAL; /// returned value
    int fd = 0;
    //  fd = open("sample_data_test.txt", O_RDONLY);

    if (argv[1] &&
            (strcmp(argv[1], "-h") || strcmp(argv[1], "--help")))
        goto exit_help;

    ret = read_graph(fd, &graph);
    close(fd);
    if (ret < 0)
        goto exit_help;

    wccs = generate_wccs(graph, ret);
    if (!wccs) {
        ret = -EINVAL;
        goto out;
    }

    print_graph(wccs);

    free_graph(wccs);
out:
    free_graph(graph);
    return ret;
exit_help:
    print_help();
    return ret;
}

