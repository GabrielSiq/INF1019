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

typedef struct node {
    DATA data;
    struct node* next;
} NODE;

void print_list(NODE* head, int pstatus);
void init(NODE** head);
NODE * add(NODE* node, DATA data);
NODE * delete_first(NODE * head);
NODE * rotate(NODE *head);
NODE * move_to_end(NODE * node, NODE * head);