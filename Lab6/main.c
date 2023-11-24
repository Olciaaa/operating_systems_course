#include <mqueue.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

struct msgbuf {
  long mtype;         /* typ komunikatu   */
  char mtext[50];      /* tresc komunikatu */
};

int main(){
    key_t key = ftok(".", '6');

    int queue = msgget(key, IPC_CREAT | 0666);

    struct msgbuf msg1 = {1, "Hellooo"};
    struct msgbuf msg2 = {1, "Forward!"};

    msgsnd(queue, &msg1, sizeof(msg1.mtext), IPC_NOWAIT);
    msgsnd(queue, &msg2, sizeof(msg2.mtext), IPC_NOWAIT);

    struct msgbuf received1;
    struct msgbuf received2;

    if (msgrcv(queue, (void *) &received1, sizeof(received1.mtext), 1, MSG_NOERROR | IPC_NOWAIT) == -1) {
            printf("Brak komunikatu dostępnego dla msgrcv()\n");
        } else{
            printf("komunikat otrzymano: %s\n", received1.mtext);
        }
}