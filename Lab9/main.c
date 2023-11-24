#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define NUM_ELVES 10

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_cond = PTHREAD_COND_INITIALIZER;

int reindeer_count = 0;
int elf_count = 0;

void* santa_claus(void* arg) {
    while (1) {
        pthread_mutex_lock(&santa_mutex);

        while (!(reindeer_count >= NUM_REINDEER || elf_count >= 3)) {
            printf("Mikołaj: zasypiam\n");
            pthread_cond_wait(&reindeer_cond, &santa_mutex);
        }

        printf("Mikołaj: budzę się\n");

        if (reindeer_count >= NUM_REINDEER) {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand() % 3 + 2);
            reindeer_count = 0;
        }

        if (elf_count >= 3) {
            printf("Mikołaj: rozwiązuje problemy elfów\n");
            sleep(rand() % 2 + 1);
            elf_count -= 3;
        }

        printf("Mikołaj: zasypiam\n");
        pthread_mutex_unlock(&santa_mutex);
    }

    return NULL;
}

void* reindeer(void* arg) {
    int reindeer_id = *(int*)arg;
    free(arg);

    while (1) {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&santa_mutex);
        printf("Renifer: %d reniferów czeka na Mikołaja\n", ++reindeer_count);

        if (reindeer_count == NUM_REINDEER) {
            printf("Renifer: wybudzam Mikołaja\n");
            pthread_cond_broadcast(&reindeer_cond);
        }

        pthread_mutex_unlock(&santa_mutex);

        sleep(rand() % 3 + 2);

        printf("Renifer: lecę na wakacje\n");
    }

    return NULL;
}

void* elf(void* arg) {
    int elf_id = *(int*)arg;
    free(arg);

    while (1) {
        sleep(rand() % 4 + 2);

        pthread_mutex_lock(&santa_mutex);

        if (elf_count < 3) {
            printf("Elf: do tej pory czekało %d elfów na Mikołaja, prychodzi nowy elf z ID: %d\n", elf_count, elf_id);
            elf_count++;

            if (elf_count == 3) {
                printf("Elf: wybudzam Mikołaja, ID: %d\n", elf_id);
                pthread_cond_broadcast(&reindeer_cond);
            }
        } else {
            printf("Elf: samodzielnie rozwiązuję swój problem, ID elfa: %d\n", elf_id);
        }

        pthread_mutex_unlock(&santa_mutex);

        if (elf_count == 3) {
            printf("Elf: Mikołaj rozwiązuje problem, ID elfa: %d\n", elf_id);
            sleep(rand() % 2 + 1);
        }

        printf("Elf: wracam do pracy, ID elfa: %d\n", elf_id);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t santa_thread;
    pthread_t reindeer_threads[NUM_REINDEER];
    pthread_t elf_threads[NUM_ELVES];

    pthread_create(&santa_thread, NULL, santa_claus, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        int* reindeer_id = malloc(sizeof(int));
        *reindeer_id = i + 1;
        pthread_create(&reindeer_threads[i], NULL, reindeer, reindeer_id);
    }

    for (int i = 0; i < NUM_ELVES; i++) {
        int* elf_id = malloc(sizeof(int));
        *elf_id = i + 1;
        pthread_create(&elf_threads[i], NULL, elf, elf_id);
    }

    pthread_join(santa_thread, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_join(reindeer_threads[i], NULL);
    }

    for (int i = 0; i < NUM_ELVES; i++) {
        pthread_join(elf_threads[i], NULL);
    }

    return 0;
}
