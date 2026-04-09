#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

// Node structure
typedef struct Node {
    char* data;
    struct Node* next;
} Node;

// Linked List structure
typedef struct {
    Node* head;
    int size;
} LinkedList;

// Function prototypes
void linked_list_init(LinkedList* list);
void linked_list_push(LinkedList* list, char* data);
char* linked_list_pop(LinkedList* list);
void linked_list_print(LinkedList* list);

#endif // LINKED_LIST_H