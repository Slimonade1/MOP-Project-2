#include "linked_list.h"
#include <stdio.h>
#include <string.h>

/**
 * Initializes the linked list
 */
void linked_list_init(LinkedList* list) {
    list->head = NULL;
    list->size = 0;
}

/**
 * Append a new node to the end of the list
 */
void linked_list_push(LinkedList* list, char* data) {
    Node* new_node = malloc(sizeof(Node));
    if(!new_node) return; // handle allocation failure
    new_node->data = data;
    new_node->next = NULL;

    if(list->head == NULL) {
        list->head = new_node;
    } else {
        Node* current = list->head;
        while (current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
    }
    list->size++;
}

/**
 *  Remove and return the tail node
 */ 
char* linked_list_pop(LinkedList* list) {
    if(list->head == NULL) return NULL; // Empty list

    if(list->head->next == NULL) {
        char* data = list->head->data;
        free(list->head);
        list->head = NULL;
        list->size--;
        return data;
    }

    Node* current = list->head;
    while(current->next->next != NULL){
        current = current->next;
    }

    Node* tail = current->next;
    char* data = tail->data;
    current->next = NULL;
    free(tail);
    list->size--;
    return data;
}

/**
 * Print the contents of the linked list
 */
void linked_list_print(LinkedList* list) {
    Node* current = list->head;
    while(current != NULL){
        printf("%s -> ", current->data);
        current = current->next;
    }
    printf("\n");
}

