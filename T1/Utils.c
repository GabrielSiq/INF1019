#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Utils.h"

int priority;

void printTime(){
	time_t rawtime;
  	struct tm * timeinfo;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	printf ( "[%d:%d:%d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  	fflush(stdout);
}

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

    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }

    if (pQueue->size >= pQueue->limit) {
        return false;
    }
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {

        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return true;
}

NODE * Dequeue(Queue *pQueue) {

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
        if(priority == true){
            printf("%s %d ", temp->data.name, temp->data.priority);
        }
        else{
            printf("%s ", temp->data.name);
        }
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

    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }

    if (pQueue->size >= pQueue->limit) {
        return false;
    }

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