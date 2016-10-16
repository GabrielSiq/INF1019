#define true 1
#define false 0

void printTime();

typedef enum {READY, RUNNING, WAITING, FINISHED} status;

typedef struct {
    int pid;
    status pstatus;
    int priority;
    char name[50];
} DATA;

typedef struct Node_t {
    DATA data;
    struct Node_t *prev;
} NODE;

/* the HEAD of the Queue, hold the amount of node's that are in the queue*/
typedef struct Queue {
    NODE *head;
    NODE *tail;
    int size;
    int limit;
} Queue;

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);
void printQueue(Queue *queue);
NODE * createNode(DATA data);
