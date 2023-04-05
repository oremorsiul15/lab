#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

pthread_t *tid;
pthread_mutex_t *stones;
bool *finished;

int N;

void *worker(void *arg)
{
    int id = *((int *)arg);

    // Move the first two stones
    pthread_mutex_lock(&stones[id]);
    pthread_mutex_lock(&stones[id + 1]);
    printf("Worker %d is moving stones %d and %d\n", id, id, id + 1);
    sleep(rand() % 3 + 1); // Simulate moving stones
    printf("Worker %d finished moving 2 stones\n", id);
    pthread_mutex_unlock(&stones[id]);
    pthread_mutex_unlock(&stones[id + 1]);

    // Move the last stone
    bool moved_last_stone = false;
    while (!moved_last_stone)
    {
        for (int i = id - 1; i <= id + 1; i++)
        {
            if (i >= 0 && i < N && !finished[i] && pthread_mutex_trylock(&stones[i]) == 0)
            {
                printf("Worker %d is moving stone %d\n", id, i);
                sleep(rand() % 3 + 1); // Simulate moving stone
                printf("Worker %d finished moving 1 stone\n", id);
                pthread_mutex_unlock(&stones[i]);
                moved_last_stone = true;
                break;
            }
        }
        usleep(100000); // Sleep for 100 ms to prevent excessive CPU usage
    }

    finished[id] = true;
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./program <number of workers>\n");
        return 1;
    }

    N = atoi(argv[1]);
    tid = (pthread_t *)malloc(N * sizeof(pthread_t));
    stones = malloc(N * sizeof(pthread_mutex_t));
    finished = malloc(N * sizeof(bool));
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_init(&stones[i], NULL);
        finished[i] = false;
    }

    int *ids = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++)
    {
        ids[i] = i;
        pthread_create(&tid[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < N; i++)
    {
        pthread_mutex_destroy(&stones[i]);
    }
    free(tid);
    free(stones);
    free(finished);
    free(ids);

    return 0;
}
