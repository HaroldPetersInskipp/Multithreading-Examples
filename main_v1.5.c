#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <math.h>

#define MAX_THREADS 8
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 2
#define NUM_FRAMES 32

/* Thread data structure */
struct thread_data {
    int thread_id;
    char *message;
};

/* Mutex for thread synchronization */
pthread_mutex_t mutex;

/* Handle for the audio device */
snd_pcm_t *handle;

/* Prints the thread ID and message */
void *print_hello(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    int taskid = my_data->thread_id;
    char *hello_msg = my_data->message;

    /* Acquire lock */
    int rc = pthread_mutex_lock(&mutex);
    if (rc) {
        printf("Error: pthread_mutex_lock, rc: %d\n", rc);
        pthread_exit(NULL);
    }

    printf("Thread ID: %d %s\n", taskid, hello_msg);

    /* Release lock */
    rc = pthread_mutex_unlock(&mutex);
    if (rc) {
        printf("Error: pthread_mutex_unlock, rc: %d\n", rc);
        pthread_exit(NULL);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[MAX_THREADS];
    struct thread_data td[MAX_THREADS];
    int status, i, num_threads;
    int error;
    short buffer[NUM_FRAMES * NUM_CHANNELS];
    
    /* Get number of threads from command line */
    if (argc < 2) {
        printf("Error: Number of threads not specified\n");
        printf("Usage: %s <num_threads>\n", argv[0]);
        exit(-1);
    }
    num_threads = atoi(argv[1]);
    if (num_threads > MAX_THREADS) {
        printf("Error: Max number of threads is %d\n", MAX_THREADS);
        exit(-1);
    }

    /* Open the audio device */
    error = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (error < 0) {
        printf("Error: snd_pcm_open, error: %d\n", error);
        exit(-1);
    }

    /* Set the audio parameters */
    error = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, NUM_CHANNELS, SAMPLE_RATE, 1, 100000);
    if (error < 0) {
        printf("Error: snd_pcm_set_params, error: %d\n", error);
        exit(-1);
    }

    /* Fill the buffer with a sine wave */
    for (i = 0; i < NUM_FRAMES * NUM_CHANNELS; i++) {
        buffer[i] = 32760 * sin(i * (2 * M_PI * 440 / SAMPLE_RATE));
    }

    /* Initialize the mutex */
    int rc = pthread_mutex_init(&mutex, NULL);
    if (rc) {
        printf("Error: pthread_mutex_init, rc: %d\n", rc);
        exit(-1);
    }

    for (i = 0; i < num_threads; i++) {
        /* Assign thread data */
        td[i].thread_id = i;
        td[i].message = "Hello from thread";

        /* Create thread */
        printf("Creating thread %d\n", i);
        status = pthread_create(&threads[i], NULL, print_hello, (void *)&td[i]);
        if (status) {
            printf("Error: pthread_create, status: %d\n", status);
            exit(-1);
        }
    }

    for (i = 0; i < num_threads; i++) {
        /* Play the audio */
        error = snd_pcm_writei(handle, buffer, NUM_FRAMES);
        if (error < 0) {
            printf("Error: snd_pcm_writei, error: %d\n", error);
            exit(-1);
        }

        /* Join thread */
        status = pthread_join(threads[i], NULL);
        if (status) {
            printf("Error: pthread_join, status: %d\n", status);
            exit(-1);
        }
    }

    /* Close the audio device */
    snd_pcm_close(handle);

    /* Destroy the mutex */
    rc = pthread_mutex_destroy(&mutex);
    if (rc) {
        printf("Error: pthread_mutex_destroy, rc: %d\n", rc);
        exit(-1);
    }

return 0;

}