// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include "linked_list.h"
#include "card.h"

char* deckOfCards[] = {
    "AH", "2H", "3H", "4H", "5H", "6H", "7H", "8H", "9H", "TH", "JH", "QH", "KH", // Hearts
    "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", // Diamonds
    "AC", "2C", "3C", "4C", "5C", "6C", "7C", "8C", "9C", "TC", "JC", "QC", "KC", // Clubs
    "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS"  // Spades
};

// Prototypes
void test_linked_list();

int main() {
    test_linked_list();
    return 0;
}

void test_linked_list(){
    LinkedList list;
    linked_list_init(&list);
    
    linked_list_push(&list, deckOfCards[0]);
    linked_list_push(&list, deckOfCards[1]);
    linked_list_push(&list, deckOfCards[2]);
    linked_list_push(&list, deckOfCards[3]);
    linked_list_push(&list, deckOfCards[4]);
    linked_list_print(&list);

    linked_list_pop(&list);
    linked_list_print(&list);

    linked_list_pop(&list);
    linked_list_print(&list);
}






