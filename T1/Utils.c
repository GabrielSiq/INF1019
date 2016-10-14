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

void print_list(NODE* head) {
    NODE * temp;
    for (temp = head; temp; temp = temp->next)
        printf("Process Id:%5d Status:%d", temp->data.pid, temp->data.pstatus);
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
    if(head != NULL){
        head = add(head, head->data);
        head = delete_first(head);
    }
    return head;
}