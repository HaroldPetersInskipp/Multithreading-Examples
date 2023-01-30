#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

struct thread_data {
    int thread_id;
    char *message;
};

void *print_hello(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    int taskid = my_data->thread_id;
    char *hello_msg = my_data->message;
    printf("%s %d\n", hello_msg, taskid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    struct thread_data td[NUM_THREADS];
    int status, i;

    for (i = 0; i < NUM_THREADS; i++) {
        printf("Creating thread %d\n", i);
        td[i].thread_id = i;
        td[i].message = "Hello from thread";
        status = pthread_create(&threads[i], NULL, print_hello, (void *)&td[i]);
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
