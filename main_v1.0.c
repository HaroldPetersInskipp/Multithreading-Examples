#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

void *print_hello(void *tid) {
    printf("Hello from thread %d\n", (int)tid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int status, i;

    for (i = 0; i < NUM_THREADS; i++) {
    	printf("Creating thread %d\n", i);
    	status = pthread_create(&threads[i], NULL, print_hello, (void *)i);
    	if (status != 0) {
    		printf("Error creating thread %d\n", i);
    		exit(-1);
   		}
   	}

   	for (i = 0; i < NUM_THREADS; i++) {
   		pthread_join(threads[i], NULL);
   	}

   	return 0;
}