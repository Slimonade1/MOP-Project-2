// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "deck_loader.h"
#include "card.h"

#define NUM_COLUMNS 7
#define NUM_CARDS 52

// Prototypes

int commandReader(Card* deckOfCards[], LinkedList* columns);

void drawToTerminal(LinkedList* columns, LinkedList* finishCells);
Card *get_card_at(LinkedList *list, int index);

void populateColumnsWithCards(LinkedList* columns, LinkedList* finishCells, Card* deckOfCards[]);
void initializeColumns(LinkedList* columns, LinkedList* finishCells);

static void free_deck(Card* deckOfCards[], int count);

// Global variables
char statusMessage[50];
char lastCommand[10];

LinkedList columns[NUM_COLUMNS];
LinkedList finishCells[4];

int main() {
    Card* deckOfCards[NUM_CARDS] = { NULL };

    initializeColumns(columns, finishCells);
    drawToTerminal(columns, finishCells);

    strcpy(statusMessage, "OK");
    commandReader(deckOfCards, columns);

    populateColumnsWithCards(columns, finishCells, deckOfCards);

    drawToTerminal(columns, finishCells);
    free_deck(deckOfCards, NUM_CARDS);
    return 0;
}

int commandReader(Card* deckOfCards[], LinkedList* columns) {
    // STARTUP phase
    char* allowedCommands[] = {"LD", "SW", "SL", "SR", "SD", "QQ", "P"};

    char input[100];
    fgets(input, sizeof(input), stdin);

    // Tokenize the input into command and argument
    char *command = strtok(input, " \t\n");
    char *argument = strtok(NULL, " \t\n");

    // Find command index
    int match = -1;
    for(int i = 0; i < sizeof(allowedCommands) / sizeof(allowedCommands[0]); i++) {
        if(strcmp(command, allowedCommands[i]) == 0) {
            match = i;
            break;
        }
    }

    switch(match) {
        case 0: // LD <filename> (loads deck into active deck)
            strcpy(lastCommand, "LD");

            int loaded = loadFile(deckOfCards, argument);
            if (loaded != NUM_CARDS) {
                strcpy(statusMessage, "Failed to load deck");
                free_deck(deckOfCards, loaded);
                return 1;
            }
            break;
        case 1: // SW (shows contents of deck)
            strcpy(lastCommand, "SW");

            for(int column = 0; column < NUM_COLUMNS; column++){
                Node *current = columns[column].head;
                while (current != NULL) {
                    Card* card = (Card*)current->data;
                    card_change_visibility(card);
                    current = current->next;
                }
            }
            break;
        case 2: // SL <split>
            // TODO Implement shuffling the cards from two seperate piles
            break;
        case 3: // SR
            // TODO Implement a random shuffle of the cards
            break;
        case 4: // SD <filename>
            // TODO Implement saving the current order of cards to a file
            break;
        case 5: // QQ
            // TODO Implement quitting the game
            break;
        case 6: // P
            // TODO Implement initializing the PLAY phase and drawing the initial game state
            break;
        default:
            strcpy(statusMessage, "Unknown command");
        }

    return 0;


    // PLAY phase
    // TODO Implement a command reader that reads commands and executes them
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

    // We need enough rows to show finishing cells
    int totalRows = maxRows > 8 ? maxRows : 8;

    for (int row = 0; row < totalRows; row++) {

        // Cards in tableau columns
        for (int col = 0; col < NUM_COLUMNS; col++) {
            Card *card = get_card_at(&columns[col], row);
            char *display = card ? (card->shown ? card->data : "[]") : "";
            printf("%-2s\t", display);
        }
        
        // Finishing cells
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
 */
void populateColumnsWithCards(LinkedList* columns, LinkedList* finishCells, Card* deckOfCards[]) {
    // Fill each column with a card
    for(int i = 0; i < NUM_CARDS; i++) {
        linked_list_push(&columns[i % NUM_COLUMNS], (void *)deckOfCards[i]);
    }
}

/**
 * Initialize coloumns
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
 * Frees the memory allocated for the deck of cards
 */
static void free_deck(Card* deckOfCards[], int count) {
    for (int i = 0; i < count; i++) {
        if (deckOfCards[i]) {
            free(deckOfCards[i]->data);
            free(deckOfCards[i]);
        }
    }
}





