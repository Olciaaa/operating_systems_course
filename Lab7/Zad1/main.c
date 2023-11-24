#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int existing_clients = 0;
key_t sem_key;
int sem_id;
key_t queue_shmem_key;
int queue_shmem_id;
key_t queue_info_shmem_key;
int queue_info_shmem_id;
int * fifo_queue;
int * fifo_info;

void initialize_sem() {
    sem_key = ftok(".", 1);
    sem_id = semget(sem_key, 2, IPC_CREAT | IPC_EXCL | 0666);

    if (sem_id == -1) {
        sem_id = semget(sem_key, 2, 0666);

        if (sem_id == -1) {
            perror("CANNOT CREATE SEMAPHORE!");
            exit(EXIT_FAILURE);
        }
        else {
            semctl(sem_id, 0, IPC_RMID);
            sem_id = semget(sem_key, 2, IPC_CREAT | IPC_EXCL | 0666);
        }
    }
}

void initialize_queue_sheme(int p){
    queue_shmem_key = ftok(".", 3);
    queue_shmem_id = shmget(queue_shmem_key, sizeof(p*sizeof(int)), IPC_CREAT | IPC_EXCL | 0666);

    if(queue_shmem_id == -1){
    queue_shmem_id = shmget(queue_shmem_key, 0, 0666);

    if(queue_shmem_id == -1){
        perror("CANNOT CREATE SHARED MEMORY!");
        exit(EXIT_FAILURE);
    }

    shmctl(queue_shmem_id, IPC_RMID, NULL);
    queue_shmem_id = shmget(queue_shmem_key, sizeof(fifo_queue), IPC_CREAT | IPC_EXCL | 0666);
    }

    queue_info_shmem_key = ftok(".", 5);
    queue_info_shmem_id = shmget(queue_info_shmem_key, sizeof(3*sizeof(int)), IPC_CREAT | IPC_EXCL | 0666);

    if(queue_info_shmem_id == -1){
        queue_info_shmem_id = shmget(queue_info_shmem_key, 0, 0666);

        if(queue_info_shmem_id == -1){
            perror("CANNOT CREATE SHARED MEMORY!");
            exit(EXIT_FAILURE);
        }

        shmctl(queue_info_shmem_id, IPC_RMID, NULL);
        queue_info_shmem_id = shmget(queue_info_shmem_key, sizeof(fifo_info), IPC_CREAT | IPC_EXCL | 0666);
    }

    fifo_info = (int *)shmat(queue_info_shmem_id, NULL, 0);
    fifo_info[0] = 0;
    fifo_info[1] = 0;
    fifo_info[2] = p;
    fifo_queue = (int *)shmat(queue_shmem_id, NULL, 0);

    if(fifo_queue == (int *)-1 || fifo_info == (int *)-1){
        perror("SHMAT FAILED!");
        exit(EXIT_FAILURE);
    }
}

void cbuff_push(int client_nr)
{
	if(fifo_info[1] >= fifo_info[2])
		fprintf(stderr, "QUEUE IS FULL!\n");
	else
	{
		fifo_queue[(fifo_info[0]+fifo_info[1])%fifo_info[2]]=client_nr;
		fifo_info[1]++;
	}
}

int is_queue_full(){
    if(fifo_info[1] >= fifo_info[2])
        return 1;
    return 0;
}

void cbuff_pop(void)
{
	if(fifo_info[1] <= 0)
		fprintf(stderr, "QUEUE IS EMPTY!\n");
	else
	{
		fifo_info[0]=(fifo_info[0]+1)%fifo_info[2];
		fifo_info[1]--;	
	}
}

int is_client_first(int client_nr){
    if(fifo_queue[fifo_info[0]] == client_nr)
        return 1;
    return 0;
}

void terminate_handler(int signalid){
    printf("Salon is closing!\n");
    shmdt(fifo_queue);
    shmctl(queue_shmem_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
    exit(0);
}


int main(int argc, char** argv){
    if(argc != 4){
        printf("3 ARGUMENTS NEEDED\n");
        return -1;
    }

    int m, n, p;

    sscanf(argv[1], "%d", &m);
    sscanf(argv[2], "%d", &n);
    sscanf(argv[3], "%d", &p);

    if(m < n){
        printf("INVALID NUMBER OF SEATS AND HAIRDRESSERS!\n");
        return -1;
    }

    initialize_sem();
    initialize_queue_sheme(p);

    union semun arg;

    arg.val = m;
    semctl(sem_id, 0, SETVAL, arg);

    arg.val = n;
    semctl(sem_id, 1, SETVAL, arg);

    struct sembuf sops[1];

    signal(SIGINT,terminate_handler);
    signal(SIGQUIT,terminate_handler);

    while(1){
        srand(time( NULL ));
        sleep(rand()%3+1);

        printf("Fryzjerow: %d | Foteli: %d | Poczekalnia: %d\n",semctl(sem_id, 0, GETVAL), semctl(sem_id, 1, GETVAL), fifo_info[2]-fifo_info[1]);

        pid_t client_id = fork();
        existing_clients++;

        if(client_id == 0){
            printf("Klient o numerze %d przyszeł do salonu\n", existing_clients);

            if(is_queue_full() == 1){
                printf("Klient o numerze %d nie znalazł miejsca w kolejce. Klient odchodzi\n", existing_clients);
            }
            else{
                sops[0].sem_op = -1;
                sops[0].sem_num = 0;
                sops[0].sem_flg = IPC_NOWAIT;

                if(semop(sem_id, sops, 1) != -1){
                    printf("Klient o numerze %d znalazł fryzjera\n", existing_clients);
                    
                    sops[0].sem_num = 1;
                    sops[0].sem_flg = 0;
                    semop(sem_id, sops, 1);

                    printf("Klient o numerze %d znalazł fotel\n", existing_clients);
                    printf("Klient o numerze %d będzie strzyzony\n", existing_clients);

                    sleep(rand()%3+10);

                    printf("Klient o numerze %d został ostrzyzony\n", existing_clients);
                }else{
                    printf("Klient o numerze %d wchodzi do kolejki\n", existing_clients);
                    cbuff_push(existing_clients);
                    
                    while(is_client_first(existing_clients) == 0){}
                    
                    printf("Klient o numerze %d jest 1 w kolejce\n", existing_clients);
                    
                    sops[0].sem_flg = 0;
                    semop(sem_id, sops, 1);

                    printf("Klient o numerze %d znalazł fryzjera\n", existing_clients);

                    sops[0].sem_num = 1;
                    semop(sem_id, sops, 1);

                    printf("Klient o numerze %d znalazł fotel\n", existing_clients);
                    printf("Klient o numerze %d będzie strzyzony\n", existing_clients);

                    sleep(rand()%3+10);

                    printf("Klient o numerze %d został ostrzyzony\n", existing_clients);

                    cbuff_pop();
                }

                sops[0].sem_op = 1;
                semop(sem_id, sops, 1);

                sops[0].sem_num = 0;
                semop(sem_id, sops, 1);
            }
            exit(0);
        }
        else if(client_id == -1){
            perror("CANNOT CREATE CHILD PROCCESS!\n");
            exit(0);
        }
    }

    return 0;
}