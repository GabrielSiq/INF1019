#define true 1
#define false 0
#define PROGRAM_LIMIT 50// Suporta uma entrada de até 50 programas com sintaxe VÁLIDA
#define CHAR_LIMIT 50
#define TIME_SLICE 2
#define IO_TIME 5

extern int priority;

void printTime();


typedef struct {
    int pid;
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
int OrderEnqueue(Queue *pQueue, NODE *item);