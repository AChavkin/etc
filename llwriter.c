#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

struct List shm_SLL_new(void *shmaddr);
void shm_SLL_push(struct List *list, const char *val);

struct Node{
    char *str;
    int stroffset;
    struct Node *next;
};

struct List{
    struct Node *head;
    struct Node *tail;
    int len;
    void *shmptr;
};

int main(int argc, char **argv){
    if(argc < 2) exit(1);

    void *shmaddr;
    if((shmaddr = shmat(atoi(argv[1]), NULL, 0)) == (void *)-1){
        printf("shmat() failed, exiting...\n");
        exit(1);
    }
    struct List list = shm_SLL_new(shmaddr);
    shm_SLL_push(&list, "World :)");
    shm_SLL_push(&list, "Hello ");
    *((int *)shmaddr) = (void *)list.head - shmaddr;

    shmdt(shmaddr);
    return 0;
}

struct List shm_SLL_new(void *shmaddr){
    struct List list;
    list.head = NULL;
    list.tail = NULL;
    list.len = 0;
    list.shmptr = shmaddr+1;
    return list;
}

void shm_SLL_push(struct List *list, const char *val){
    struct Node new;
    strcpy(list->shmptr, val);
    new.str = list->shmptr;
    new.next = NULL;
    new.stroffset = strlen(val)+1;
    list->shmptr += (strlen(val)+1);
    *((struct Node *)list->shmptr) = new;
    if(list->len == 0){
        list->head = list->shmptr;
        list->tail = list->shmptr;
        list->shmptr += sizeof(struct Node);
        (list->len)++;
        return;
    }
    new.next = list->head;
    list->head = list->shmptr;
    list->shmptr += sizeof(struct Node);
}
