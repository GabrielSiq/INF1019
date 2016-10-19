#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Utils.h"

void printTime(){
	time_t rawtime;
  	struct tm * timeinfo;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	printf ( "[%d:%d:%d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  	fflush(stdout);
}

//---------------------------------- List --------------------

// void init(NODE** head) {
//     *head = NULL;
// }

// void print_list(NODE* head, int pstatus) {
//     NODE * temp;
//     for (temp = head; temp; temp = temp->next){
//         if(pstatus < 0 || temp->data.pstatus == pstatus){
//             printf("%s Status:%d ", temp->data.name, temp->data.pstatus);
//         }
//     }
//     printf("\n");
// }

// NODE* add(NODE* node, DATA data) {
//     int i;
//     NODE* temp = (NODE*) malloc(sizeof (NODE));
//     if (temp == NULL) {
//         exit(0); 
//     }

//     temp->data = data;
//     temp->next = NULL;

//     if(node == NULL){
//         node = temp;
//     }
//     else if(node->next == NULL){
//         node->next = temp;
//     }
//     else{
//         NODE *current = node;
//         while(true){
//             if(current->next == NULL){
//                 current->next = temp;
//                 break;
//             }
//             current = current->next;
//         }
//     }
    
//     return node;
// }

// void add_at(NODE* node, DATA data) {
//     NODE* temp = (NODE*) malloc(sizeof (NODE));
//     if (temp == NULL) {
//         exit(EXIT_FAILURE); 
//     }
//     temp->data = data;
//     temp->next = node->next;
//     node->next = temp;
// }

// NODE * delete_first(NODE * head) {
//     NODE* temp = (NODE*) malloc(sizeof (NODE));
//     if (temp == NULL) {
//         exit(EXIT_FAILURE); 
//     }
//     if(head != NULL){
//         temp = head;
//         head = temp->next;
//         free(temp);
//     }
//     return head;
// }

// NODE * reverse_rec(NODE * ptr, NODE * previous) {
//     NODE * temp;
//     if (ptr->next == NULL) {
//         ptr->next = previous;
//         return ptr;
//     } else {
//         temp = reverse_rec(ptr->next, ptr);
//         ptr->next = previous;
//         return temp;
//     }
// }

// NODE * reverse(NODE * node) {
//     NODE * temp;
//     NODE * previous = NULL;
//     while (node != NULL) {
//         temp = node->next;
//         node->next = previous;
//         previous = node;
//         node = temp;
//     }
//     return previous;
// }

// NODE *free_list(NODE *head) {
//     NODE *tmpPtr = head;
//     NODE *followPtr;
//     while (tmpPtr != NULL) {
//         followPtr = tmpPtr;
//         tmpPtr = tmpPtr->next;
//         free(followPtr);
//     }
//     return NULL;
// }

// NODE * rotate(NODE *head){
//     NODE * next, * current = head;
//     if(head != NULL){
//         while(current->next != NULL){
//             current = current->next;
//         }
//         current->next = head;
//         next = head->next;
//         head->next = NULL;
//         return next;
//     }
//     return head;
// }

// NODE* move_to_end(NODE * node, NODE * head){
//     NODE * current, * current2, * next;
//     // Se o nó for vazio, a lista for vazia, ou o nó ja for o último da lista, não faça nada
//     if(node == NULL || head == NULL || node->next == NULL){
//         return head;
//     }
//     // Se estivermos movendo o primeiro nó da lista, rodamos a lista
//     if(head == node){
//         return rotate(head);
//     }
//     current = node;
//     // Faz o último item da lista apontar pra ele
//     while(current->next != NULL){
//         current = current->next;
//     }
//     current->next = node;

//     // Faz o anterior apontar pro próximo e ele apontar pra NULL
//     current = head;
//     while(current->next != node){
//         current = current->next;
//     }
//     current->next = node->next;
//     node->next = NULL;
//     return head;
// }

// --------------------- Queue --------------------- //


Queue *ConstructQueue(int limit) {
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        return NULL;
    }
    if (limit <= 0) {
        limit = 65535;
    }
    queue->limit = limit;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void DestructQueue(Queue *queue) {
    NODE *pN;
    while (!isEmpty(queue)) {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);
}

int Enqueue(Queue *pQueue, NODE *item) {
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return false;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the end of the queue*/
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return true;
}

NODE * Dequeue(Queue *pQueue) {
    /*the queue is empty or bad param*/
    NODE *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;
    return item;
}

int isEmpty(Queue* pQueue) {
    if (pQueue == NULL) {
        return false;
    }
    if (pQueue->size == 0) {
        return true;
    } else {
        return false;
    }
}

void printQueue(Queue *queue){
    NODE * temp;
    int i=0;
    for (temp = queue->head; temp; temp = temp->prev){
        printf("%s %d ", temp->data.name, temp->data.priority);
        if(i > queue->size){
            break;
        }
        i++;
    }
    printf("\n");
}

NODE * createNode(DATA data){
    NODE * temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(0); 
    }
    temp->data = data;
    temp->prev = NULL;
    return temp;
}

int OrderEnqueue(Queue *pQueue, NODE *item) {
    NODE * temp;
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return false;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;
        pQueue->size = 1;
    } else {
        if(pQueue->tail->data.priority <= item->data.priority){
            Enqueue(pQueue, item);
            return true;
        }else{
            if(pQueue->size == 1){
                Enqueue(pQueue, item);
                temp = Dequeue(pQueue);
                Enqueue(pQueue, temp);
                return true;
            }
        }
        while(pQueue->head->data.priority <= item->data.priority){
            temp = Dequeue(pQueue);
            Enqueue(pQueue, temp);
        }
        Enqueue(pQueue, item);
        while(pQueue->head->data.priority <= pQueue->head->prev->data.priority){
            temp = Dequeue(pQueue);
            Enqueue(pQueue, temp);
        }
        temp = Dequeue(pQueue);
        Enqueue(pQueue, temp);
        return true;
    }
}