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

#define NUM_COLUMNS 7
#define NUM_CARDS 52

// Prototypes
void drawToTerminal(LinkedList* columns);
void populateColumns(LinkedList* columns);
char *get_card_at(LinkedList *list, int index);


int main() {
    LinkedList columns[NUM_COLUMNS];
    
    populateColumns(columns);
    drawToTerminal(columns);

    return 0;
}

/**
 * Draws the GUI of the game to the console
 */
void drawToTerminal(LinkedList* columns) {
    // Print column headers
    printf("c1\tc2\tc3\tc4\tc5\tc6\tc7\n");
    printf("\n");

    int maxRows = 0;
    for(int i = 0; i < NUM_COLUMNS; i++) {
        if(columns[i].size > maxRows) {
            maxRows = columns[i].size;
        }
    }

    // print the cards row by row
    for(int row = 0; row < maxRows; row++) {
        for(int col = 0; col < NUM_COLUMNS; col++) {
            char *card = get_card_at(&columns[col], row);
            printf("%-3s\t", card ? card : "");
        }
        printf("\n");
    }
}

/**
 * Helper function to get card at specific index
 */
char *get_card_at(LinkedList *list, int index) {
    Node *cur = list->head;
    int i = 0;
    while(cur && i < index) {
        cur = cur->next;
        i++;
    }
    return cur ? cur->data : NULL;
}

/**
 * Initialize coloumns and populate with cards
 */
void populateColumns(LinkedList* columns) {
    // Initialize all columns
    for(int i = 0; i < NUM_COLUMNS; i++) {
        linked_list_init(&columns[i]);
    }

    // Fill each column with a card
    for(int i = 0; i < NUM_CARDS; i++) {
        linked_list_push(&columns[i % NUM_COLUMNS], deckOfCards[i]);
    }
}





