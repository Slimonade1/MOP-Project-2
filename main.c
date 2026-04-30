// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "linked_list.h"
#include "deck_loader.h"
#include "card.h"
#include "command_reader.h"
#include "game.h"

// Global variables
#define NUM_COLUMNS 7
#define NUM_CARDS 52

char statusMessage[50];
char lastCommand[10];

LinkedList columns[NUM_COLUMNS];
LinkedList finishCells[4];

// Prototypes

void drawToTerminal(LinkedList* columns, LinkedList* finishCells);
Card *get_card_at(LinkedList *list, int index);

void initializeColumns(LinkedList* columns, LinkedList* finishCells);

int main() {
    GamePhase phase = GAME_STARTUP;

    LinkedList deckOfCards;
    linked_list_init(&deckOfCards);

    initializeColumns(columns, finishCells);
    srand((unsigned)time(NULL)); // create new seed / randomize game

    while(phase != GAME_QUIT) {
        drawToTerminal(columns, finishCells); // Empty grid start of game
        strcpy(statusMessage, "OK");

        switch(phase) {
            case GAME_STARTUP:
                phase = commandReaderStartup(&deckOfCards, columns, statusMessage, lastCommand);
                break;
            
            case GAME_PLAY:
                phase = commandReaderPlay(columns, finishCells, statusMessage, lastCommand);
                break;
            
            default:
                phase = GAME_QUIT;
                break;
        }
    }

    freeDeck(&deckOfCards);
    return 0;
}

/**
 * Draws the GUI of the game to the console
 */
void drawToTerminal(LinkedList *columns, LinkedList finishCells[4]) {

    printf("C1\tC2\tC3\tC4\tC5\tC6\tC7\n\n");

    // Find tallest tableau column
    int maxRows = 0;
    for (int i = 0; i < NUM_COLUMNS; i++) {
        if (columns[i].size > maxRows)
            maxRows = columns[i].size;
    }

    // We need enough rows to show foundation cells
    int totalRows = maxRows > 8 ? maxRows : 8;

    for (int row = 0; row < totalRows; row++) {

        // Cards in tableau columns
        for (int col = 0; col < NUM_COLUMNS; col++) {
            Card *card = get_card_at(&columns[col], row);
            char *display = card ? (card->shown ? card->data : "[]") : "";
            printf("%-2s\t", display);
        }
        
        // Foundation cells
        if (row % 2 == 0) {
            int fIndex = row / 2;

            if (fIndex < 4) {
                Card *tail = get_tail_card(&finishCells[fIndex]);
                char *display = (tail == NULL) ? "[]" : tail->data;

                printf("\t%-2s  F%d", display, fIndex + 1);
            }
        }
        printf("\n");
    }

    printf("\nLAST Command: %s\n", lastCommand);
    printf("STATUS: %s\n", statusMessage);
    printf("INPUT > ");
}


/**
 * Helper function to get card at specific index
 */
Card *get_card_at(LinkedList *list, int index) {
    Node *current = list->head;
    int i = 0;
    while(current && i < index) {
        current = current->next;
        i++;
    }

    return current ? (Card *)current->data : NULL;
}

/**
 * Populate the columns with cards from the deck in a round-robin manner 
 * in the startup phase
 */
void dealCards(LinkedList* deckOfCards, LinkedList* columns) {
    // Remove cards if they exist
    for(int i = 0; i < NUM_COLUMNS; i++){
        while(columns[i].head != NULL) linked_list_pop_tail(&columns[i]);
    }

    Node *current = deckOfCards->head;
    int i = 0;

    // Fill each column with cards
    while (current) {
        linked_list_push(&columns[i % NUM_COLUMNS], current->data);
        current = current->next;
        i++;
    }
}

/**
 * Initialises all columns and finish cells
 */
void initializeColumns(LinkedList* columns, LinkedList* finishCells) {
    // Initialize all columns
    for(int i = 0; i < NUM_COLUMNS; i++) {
        linked_list_init(&columns[i]);
    }

    // Initialize finish cells with a blank card
    for(int i = 0; i < 4; i++) {
        linked_list_init(&finishCells[i]);
    }
}

/**
 * Empties the deck of any cards
 */
void freeDeck(LinkedList *deckOfCards) {
    while (deckOfCards->head != NULL) {
        Card *card = linked_list_pop_tail(deckOfCards);
        free(card->data);
        free(card);
    }
}

