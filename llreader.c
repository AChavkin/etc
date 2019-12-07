#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Node{
    char *val;
    int stroffset;
    struct Node *next;
};

struct List{
    struct Node *head;
};

int shm_SLL_pop(struct List *list);
struct List shm_SLL_retrieve(struct Node *head);

int main(){
    int shm;
    int status;
    char sh[20];
    if((shm = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666)) == -1){
        printf("shmget() failed, exiting...\n");
        exit(1);
    }
    pid_t pid = fork();

    switch(pid){
        case -1: 
            printf("Fork failed, exiting...\n");
            exit(1);
        case 0:
            sprintf(sh, "%d", shm);
            execl("./llwriter", "llwriter", sh,  NULL);
            printf("Couldn't exec, exiting...\n");
            exit(1);
        default:
            wait(&status);
    }

    void *shmaddr;
    if((shmaddr = shmat(shm, NULL, 0)) == (void *)-1){
        printf("shmat() failed, exiting...\n");
        exit(1);
    }
    int offset = *((int *)shmaddr);
    int stroffset = ((struct Node *)(shmaddr + offset))->stroffset;
    printf("PARENT: shm says %s\n", ((struct Node *)(shmaddr+offset-stroffset)));
    
    shmdt(shmaddr);
    shmctl(shm, IPC_RMID, NULL);

    return 0;
}

int shm_SLL_pop(struct List *list){
    return 0;
}

struct List shm_SLL_retrieve(struct Node *head){
    struct List list;
    list.head = head;
    return list;
}
