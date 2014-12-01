#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

static int read_line_safe(int fd, char *buf, int size)
{
	int i = 0;
	int ret;

	for (i = 0; i < size - 2; ++i) {
		ret = read(fd, &(buf[i]), 1);
		if (ret < 1)
			return -errno;
		if (buf[i] == '\n')
			break;
	}

	ret = buf[i] == '\n' ? 0 : -EINVAL;
	buf[i+1] = '\0';
	return ret;
}

static int count_conn(char *buf)
{
	int i;
	int n_elem = 0;

	for (i = 0; buf[i] != '\0';) {
		while (buf[i] != '\0' && isspace(buf[i]))
			++i;

		if (buf[i] == '\0')
			break;

		++n_elem;
		while (buf[i] != '\0' && !isspace(buf[i]))
			++i;
	}

	return n_elem;
}

static int format_token(char *buf, char **next)
{
	int i = 0;
	int ret = -EINVAL;

	if (!buf)
		goto out;


	while (buf[i] != '\0' && isspace(buf[i]))
		++i;

	if (buf[i] == '\0')
		goto out;

	while (buf[i] != '\0' && !isspace(buf[i]))
		++i;

	buf[i++] = '\0';
	*next = buf[i] != '\0' ? buf + i : NULL;
	ret = 0;

out:
	return ret;

}

int read_graph(int fd, int ***graph)
{
	int n_verticles;
	int ret;
	int i;
	int **g;
	char buf[4096];

	ret = read_line_safe(fd, buf, sizeof(buf));
	if (ret)
		goto out;

	ret = sscanf(buf, "%d", &n_verticles);
	if (ret != 1) {
		ret = -1;
		goto out;
	}

	g = calloc(n_verticles + 1, sizeof(*g));
	if (!g) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < n_verticles; ++i) {
		int n_conn;
		char *next, *curr;
		int conn;
		int j;

		ret = read_line_safe(fd, buf, sizeof(buf));
		if (ret)
			goto free_memory;

		n_conn = count_conn(buf);
		g[i] = calloc(n_conn + 1, sizeof(*(g[i])));
		if (!g[i]) {
			ret = -ENOMEM;
			goto free_memory;
		}

		for (j = 0, curr = buf; !format_token(curr, &next); ++j) {
			ret = sscanf(curr, "%d", &conn);
			if (ret != 1) {
				ret = -EINVAL;
				goto free_memory;
			}

			if (conn < 0 || conn > n_verticles - 1) {
				ret = -EINVAL;
				goto free_memory;
			}

			g[i][j] = conn;
			curr = next;
		}
		g[i][j] = -1;

	}

	*graph = g;
	return n_verticles;

free_memory:
	for (i = 0; g[i]; ++i)
		free(g[i]);

	free(g[i]);
out:
	return ret;
}

void free_graph(int **graph)
{
	int **g;
	if (!graph)
		return;
	for (g = graph; *g; ++g)
		free(*g);

	free(graph);
}

void print_graph(int **graph)
{
	int **g;
	int *v;
	int i = 0;

	for (g = graph; *g; ++g)
		++i;

	printf("%d\n", i);

	for (g = graph; *g; ++g) {
		for (v = *g; *v >= 0; ++v)
			printf("%d ", *v);
		putchar('\n');
	}
}

