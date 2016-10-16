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

void init(NODE** head) {
    *head = NULL;
}

void print_list(NODE* head, int pstatus) {
    NODE * temp;
    for (temp = head; temp; temp = temp->next){
        if(pstatus < 0 || temp->data.pstatus == pstatus){
            printf("%s Status:%d ", temp->data.name, temp->data.pstatus);
        }
    }
    printf("\n");
}

NODE* add(NODE* node, DATA data) {
    int i;
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(0); 
    }

    temp->data = data;
    temp->next = NULL;

    if(node == NULL){
        node = temp;
    }
    else if(node->next == NULL){
        node->next = temp;
    }
    else{
        NODE *current = node;
        while(true){
            if(current->next == NULL){
                current->next = temp;
                break;
            }
            current = current->next;
        }
    }
    
    return node;
}

void add_at(NODE* node, DATA data) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(EXIT_FAILURE); 
    }
    temp->data = data;
    temp->next = node->next;
    node->next = temp;
}

NODE * delete_first(NODE * head) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(EXIT_FAILURE); 
    }
    if(head != NULL){
        temp = head;
        head = temp->next;
        free(temp);
    }
    return head;
}

NODE * reverse_rec(NODE * ptr, NODE * previous) {
    NODE * temp;
    if (ptr->next == NULL) {
        ptr->next = previous;
        return ptr;
    } else {
        temp = reverse_rec(ptr->next, ptr);
        ptr->next = previous;
        return temp;
    }
}

NODE * reverse(NODE * node) {
    NODE * temp;
    NODE * previous = NULL;
    while (node != NULL) {
        temp = node->next;
        node->next = previous;
        previous = node;
        node = temp;
    }
    return previous;
}

NODE *free_list(NODE *head) {
    NODE *tmpPtr = head;
    NODE *followPtr;
    while (tmpPtr != NULL) {
        followPtr = tmpPtr;
        tmpPtr = tmpPtr->next;
        free(followPtr);
    }
    return NULL;
}

NODE * rotate(NODE *head){
    NODE * next, * current = head;
    if(head != NULL){
        while(current->next != NULL){
            current = current->next;
        }
        current->next = head;
        next = head->next;
        head->next = NULL;
        return next;
    }
    return head;
}

NODE* move_to_end(NODE * node, NODE * head){
    NODE * current, * current2, * next;
    // Se o nó for vazio, a lista for vazia, ou o nó ja for o último da lista, não faça nada
    if(node == NULL || head == NULL || node->next == NULL){
        return head;
    }
    // Se estivermos movendo o primeiro nó da lista, rodamos a lista
    if(head == node){
        return rotate(head);
    }
    current = node;
    // Faz o último item da lista apontar pra ele
    while(current->next != NULL){
        current = current->next;
    }
    current->next = node;

    // Faz o anterior apontar pro próximo e ele apontar pra NULL
    current = head;
    while(current->next != node){
        current = current->next;
    }
    current->next = node->next;
    node->next = NULL;
    return head;
}