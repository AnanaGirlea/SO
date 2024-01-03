// SPDX-License-Identifier: BSD-3-Clause

#include "os_threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* === TASK === */

/* Creates a task that thread must execute */
os_task_t *task_create(void *arg, void (*f)(void *))
{
	// TODO - DONE
	os_task_t *t = malloc(sizeof(os_task_t));

	t->argument = arg;
	t->task = f;

	return t;
}

/* Add a new task to threadpool task queue */
void add_task_in_queue(os_threadpool_t *tp, os_task_t *t)
{
	// TODO - DONE
	os_task_queue_t *qt = malloc(sizeof(os_task_queue_t));

	qt->task = t;
	qt->next = NULL;

	pthread_mutex_lock(&tp->taskLock);

	if (tp->tasks) {
		os_task_queue_t *q = tp->tasks;

		while (q->next)
			q = q->next;
		q->next = qt;
	} else {
		tp->tasks = qt;
	}

	pthread_mutex_unlock(&tp->taskLock);
}

/* Get the head of task queue from threadpool */
os_task_t *get_task(os_threadpool_t *tp)
{
	// TODO - DONE
	os_task_t *t = NULL;

	pthread_mutex_lock(&tp->taskLock);

	if (tp->tasks) {
		t = tp->tasks->task;
		tp->tasks = tp->tasks->next;
	}

	pthread_mutex_unlock(&tp->taskLock);

	return t;
}

/* === THREAD POOL === */

/* Initialize the new threadpool */
os_threadpool_t *threadpool_create(unsigned int nTasks, unsigned int nThreads)
{
	// TODO - DONE
	int i;
	os_threadpool_t *tp = malloc(sizeof(os_threadpool_t));

	tp->should_stop = 0;
	tp->num_threads = nThreads;
	tp->threads = malloc(nThreads * sizeof(pthread_t));
	tp->tasks = NULL;
	pthread_mutex_init(&tp->taskLock, NULL);

	for (i = 0; i < nThreads; i++)
		pthread_create(&tp->threads[i], NULL, thread_loop_function, tp);

	return tp;
}

/* Loop function for threads */
void *thread_loop_function(void *args)
{
	// TODO - DONE
	os_threadpool_t *tp = (os_threadpool_t *) args;

	while (tp->tasks || !tp->should_stop) {
		os_task_t *t = get_task(tp);

		if (t)
			t->task(t->argument);
	}
}

/* Stop the thread pool once a condition is met */
void threadpool_stop(os_threadpool_t *tp, int (*processingIsDone)(os_threadpool_t *))
{
	// TODO - DONE
	int i;
	void **sol = NULL;

	while (1)
		if (processingIsDone(tp))
			break;
	tp->should_stop = 1;

	for (i = 0; i < tp->num_threads; i++)
		pthread_join(tp->threads[i], sol);
}
