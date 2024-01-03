// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 100
#define MAX_THREAD 4

static int sum;
static os_graph_t *graph;
os_threadpool_t *tp;
pthread_mutex_t sumLock;
pthread_mutex_t grafLock;

// Functia calculeaza suma si creaza task-uri pentru vecinii nodului curent.
// Am pus mutex pentru suma pentru ca mai multe thread-uri pot sa acceseze suma in acelasi timp
//	si ea nu se va mai calcula corect.
// Am pus mutex pentru vizitarea nodurilor pentru a evita vizitarea aceluiasi nod in acelasi timp.
void processNode(void *arg)
{
	int i;
	unsigned int id = *(unsigned int *)arg;
	os_node_t *node = graph->nodes[id];

	pthread_mutex_lock(&sumLock);
	sum = sum + node->nodeInfo;
	pthread_mutex_unlock(&sumLock);

	for (i = 0; i < node->cNeighbours; i++) {
		pthread_mutex_lock(&grafLock);
		if (graph->visited[node->neighbours[i]] == 0) {
			graph->visited[node->neighbours[i]] = 1;

			os_task_t *t = task_create(&node->neighbours[i], processNode);

			add_task_in_queue(tp, t);
		}
		pthread_mutex_unlock(&grafLock);
	}
}

// Functia traverseaza graful si creaza task-uri pentru nodurile nevizitate.
// Am pus mutex pentru vizitarea nodurilor pentru a evita vizitarea aceluiasi nod in acelasi timp.
static void traverse_graph(void)
{
	int i;

	for (i = 0; i < graph->nCount; i++) {
		pthread_mutex_lock(&grafLock);
		if (graph->visited[i] == 0) {
			graph->visited[i] = 1;

			unsigned int *k = malloc(sizeof(unsigned int));
			*k = i;
			os_task_t *t = task_create(k, processNode);

			add_task_in_queue(tp, t);
		}
		pthread_mutex_unlock(&grafLock);
	}
}

// Functia processingIsDone pentru threadpool_stop.
// Functia care opreste threadpool-ul daca toate nodurile au fost vizitate.
int stoptp(os_threadpool_t *tp)
{
	int i, ok = 0;

	for (i = 0; i < graph->nCount; i++) {
		if (graph->visited[i] == 0) {
			ok = 1;
			break;
		}
	}
	if (ok == 0)
		return 1;
	return 0;
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	graph = create_graph_from_file(input_file);
	if (graph == NULL) {
		fprintf(stderr, "[Error] Can't read the graph from file\n");
		return -1;
	}

	// TODO: create thread pool and traverse the graf
	pthread_mutex_init(&sumLock, NULL);
	pthread_mutex_init(&grafLock, NULL);

	tp = threadpool_create(MAX_TASK, MAX_THREAD);

	traverse_graph();
	threadpool_stop(tp, stoptp);

	printf("%d", sum);

	return 0;
}
