#include "linked_list.h"
#include "card.h"
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
void linked_list_push(LinkedList* list, void* data) {
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

void linked_list_insert_at(LinkedList* list, void* data, int index) {
    if (index < 0 || index > list->size) {
            return; // index out of bounds
    }

    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    new_node->next = NULL;

    // Insert at head
    if (index == 0) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;
        return;
    }

    // Traverse to node before insertion point
    Node* current = list->head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
    list->size++;
}

/**
 *  Remove and return the tail node
 */ 
void* linked_list_pop_tail(LinkedList* list) {
    if(list->head == NULL) return NULL; // Empty list

    if(list->head->next == NULL) {
        void* data = list->head->data;
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
    void* data = tail->data;
    current->next = NULL;
    free(tail);
    list->size--;
    return data;
}

/**
 * Remove and return the head node
 */
void* linked_list_pop_front(LinkedList *list) {
    if (!list->head) return NULL;

    Node *node = list->head;
    void *data = node->data;

    list->head = node->next;
    free(node);
    list->size--;

    return data;
}

/**
 * Return the data of the tail node without removing it
 */
void* get_tail_card(LinkedList *list) {
    if (list->head == NULL) return NULL; // Empty list

    Node* current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current->data;
}

