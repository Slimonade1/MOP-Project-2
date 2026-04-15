// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "linked_list.h"
#include "deck_loader.h"
#include "card.h"

#define NUM_COLUMNS 7
#define NUM_CARDS 52

// Prototypes

int commandReader(LinkedList *deckOfCards);
void showAllCards(LinkedList *deckOfCards);
void shuffleSplit(LinkedList *deckOfCards, int split);
void shuffleRandom(LinkedList *deckOfCards);

void drawToTerminal(LinkedList* columns, LinkedList* finishCells);
Card *get_card_at(LinkedList *list, int index);

void initializeColumns(LinkedList* columns, LinkedList* finishCells);
void updateColumns(LinkedList *deck, LinkedList *columns);
void freeDeck(LinkedList *deckOfCards);

// Global variables
char statusMessage[50];
char lastCommand[10];

LinkedList columns[NUM_COLUMNS];
LinkedList finishCells[4];

int main() {
    LinkedList deckOfCards;
    linked_list_init(&deckOfCards);
    srand((unsigned)time(NULL)); // create new seed

    // Empty grid start of game
    initializeColumns(columns, finishCells);
    drawToTerminal(columns, finishCells);

    while(true) { // TEMP ALWAYS TRUE, switch with command "QQ"
        strcpy(statusMessage, "OK");
        commandReader(&deckOfCards);
        drawToTerminal(columns, finishCells);
    }
    return 0;
}

/**
 *  Reads commands from the terminal
 * 
 *  ... TODO write more documentation when function is finished
 */
int commandReader(LinkedList *deckOfCards) {
    // STARTUP phase
    char* allowedCommands[] = {"LD", "SW", "SI", "SR", "SD", "QQ", "P"};

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

            freeDeck(deckOfCards);
            linked_list_init(deckOfCards);

            int loaded = loadFile(deckOfCards, argument);
            if (loaded != NUM_CARDS) {
                strcpy(statusMessage, "Failed to load deck");
                freeDeck(deckOfCards);
                return 1;
            }

            updateColumns(deckOfCards, columns);
            break;

        case 1: // SW (shows contents of deck)
            strcpy(lastCommand, "SW");
            showAllCards(deckOfCards);
            break;

        case 2: // SI <split> (shuffles the deck from a given index)
            strcpy(lastCommand, "SI");
            
            int split;

            if(argument == NULL) {
                // If no argument was given, split at random position
                split = rand() % (NUM_CARDS - 1) + 1; // 1 .. NUM_CARDS-1
            } else {
                char *end;
                split = (int)strtol(argument, &end, 10);
                if (end == argument || *end != '\0') {
                    strcpy(statusMessage, "Argument must be an int");
                    return 1;
                }

                if(split <= 0 || split >= NUM_CARDS) {
                    strcpy(statusMessage, "Index out of bounds");
                    return 1;
                }
            }

            shuffleSplit(deckOfCards, split);
            updateColumns(deckOfCards, columns);
            break;

        case 3: // SR (shuffles the deck to a random deck)
            strcpy(lastCommand, "SR");
            shuffleRandom(deckOfCards);
            updateColumns(deckOfCards, columns);
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
 * Shuffles a deck of cards in place using random insertion on a linked list.
 *
 * @param deckOfCards Pointer to the linked-list deck to be shuffled.
 */
void shuffleRandom(LinkedList *deckOfCards) {
    LinkedList shuffled;
    linked_list_init(&shuffled);

    while (deckOfCards->head) {
        // Remove card from top of deck
        void *card = linked_list_pop_front(deckOfCards);

        // Choose random insertion position
        int pos = rand() % (shuffled.size + 1);

        // Insert card data into shuffled deck
        linked_list_insert_at(&shuffled, card, pos);
    }

    *deckOfCards = shuffled;
}

/**
 * Changes all cards to be shown in the UI
 */
void showAllCards(LinkedList *deckOfCards) {
    Node *current = deckOfCards->head;

    while(current) {
        Card *card = current->data;
        card->shown = true;
        current = current->next;
    }
}

/**
 * Shuffles the deck by splitting it into two piles and interleaving their cards.
 *
 * The deck is split at the given index and cards are alternately taken from each
 * pile to form a new shuffled deck.
 *
 * @param deckOfCards LinkedList of {@code NUM_CARDS} length with cards
 * @param split Index at which the deck is split (0 < split < NUM_CARDS).
 */
void shuffleSplit(LinkedList *deckOfCards, int split) {
    LinkedList a, b, shuffled;
    linked_list_init(&a);
    linked_list_init(&b);
    linked_list_init(&shuffled);

    // Split the deck into two piles
    for (int i = 0; i < split; i++) {
        linked_list_push(&a, linked_list_pop_front(deckOfCards));
    }

    while (deckOfCards->head) {
        linked_list_push(&b, linked_list_pop_front(deckOfCards));
    }

    // Interleave: take from A then B
    while (a.head && b.head) {
        linked_list_push(&shuffled, linked_list_pop_front(&a));
        linked_list_push(&shuffled, linked_list_pop_front(&b));
    }

    // Append remainder
    while (a.head) linked_list_push(&shuffled, linked_list_pop_front(&a));
    while (b.head) linked_list_push(&shuffled, linked_list_pop_front(&b));

    *deckOfCards = shuffled;

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
void updateColumns(LinkedList* deckOfCards, LinkedList* columns) {
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




