/* We use global variables to allow our recursive functions go deeper */
static char *visited = NULL;
static int *new_order = NULL;



static int *mark_nodes(int **graph, int size)
{
	int i;

	visited = calloc(size, sizeof(*new_order));
	new_order = calloc(size, sizeof(*new_order));

	if (!visited || !new_order) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < size; ++i) 

out:
	return ret;
}
