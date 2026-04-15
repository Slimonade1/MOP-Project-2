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

int commandReader(Card* deckOfCards[], LinkedList* columns);
void showAllCards(Card *deckOfCards[]);
void shuffleSplit(Card *deckOfCards[], int indexOfSplit);
void shuffleRandom(Card* deckOfCards[]);

void drawToTerminal(LinkedList* columns, LinkedList* finishCells);
Card *get_card_at(LinkedList *list, int index);

void initializeColumns(LinkedList* columns, LinkedList* finishCells);
void updateColumns(LinkedList* columns, Card* deckOfCards[]);

static void free_deck(Card* deckOfCards[], int count);

// Global variables
char statusMessage[50];
char lastCommand[10];

LinkedList columns[NUM_COLUMNS];
LinkedList finishCells[4];

int main() {
    Card* deckOfCards[NUM_CARDS] = { NULL };
    srand((unsigned)time(NULL)); // create new seed

    // Empty grid start of game
    initializeColumns(columns, finishCells);
    drawToTerminal(columns, finishCells);

    while(true) { // TEMP ALWAYS TRUE, switch with command "QQ"
        strcpy(statusMessage, "OK");
        commandReader(deckOfCards, columns);
        drawToTerminal(columns, finishCells);
    }

    free_deck(deckOfCards, NUM_CARDS);
    return 0;
}

/**
 *  Reads commands from the terminal
 * 
 *  ... TODO write more documentation when function is finished
 */
int commandReader(Card* deckOfCards[], LinkedList* columns) {
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

            int loaded = loadFile(deckOfCards, argument);
            if (loaded != NUM_CARDS) {
                strcpy(statusMessage, "Failed to load deck");
                free_deck(deckOfCards, loaded);
                return 1;
            }

            updateColumns(columns, deckOfCards);
            break;

        case 1: // SW (shows contents of deck)
            strcpy(lastCommand, "SW");
            showAllCards(deckOfCards);
            break;

        case 2: // SI <split>
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
            updateColumns(columns, deckOfCards);
            break;

        case 3: // SR (shuffles the deck to a random deck)
            strcpy(lastCommand, "SR");
            shuffleRandom(deckOfCards);
            updateColumns(columns, deckOfCards);
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
 * Randomly shuffles a deck of cards using the random insertion method.
 *
 * Each card from the original deck is inserted into a temporary pile at a
 * randomly chosen position, then copied back into the original array.
 *
 * @param deckOfCards Array of {@code NUM_CARDS} pointers to Card objects.
 */
void shuffleRandom(Card* deckOfCards[]) {
    Card* shuffledDeck[NUM_CARDS] = { NULL }; 
    int shuffled_size = 0;

    for(int i = 0; i < NUM_CARDS; i++) {
        // Choose a random position from 0 to shuffled_size
        int pos = rand() % (shuffled_size + 1);

        // Shift cards to the right to make room
        for(int j = shuffled_size; j > pos; j--){
            shuffledDeck[j] = shuffledDeck[j - 1];
        }

        // Insert the new card
        shuffledDeck[pos] = deckOfCards[i];
        shuffled_size++;
    }

    // Copy shuffled deck back into deckOfCards
    for(int i = 0; i < NUM_CARDS; i++){
        deckOfCards[i] = shuffledDeck[i];
    }
}
/**
 * Changes all cards to be shown in the UI
 */
void showAllCards(Card *deckOfCards[]) {
    for(int column = 0; column < NUM_COLUMNS; column++){
        Node *current = columns[column].head;
        while (current != NULL) {
            Card* card = (Card*)current->data;
            card_change_visibility(card);
            current = current->next;
        }
    }
}

/**
 * Shuffles the deck by splitting it into two piles and interleaving their cards.
 *
 * The deck is split at the given index and cards are alternately taken from each
 * pile to form a new shuffled deck.
 *
 * @param deckOfCards Array of {@code NUM_CARDS} pointers to Card objects.
 * @param split Index at which the deck is split (0 < split < NUM_CARDS).
 */
void shuffleSplit(Card *deckOfCards[], int indexOfSplit) {
    Card *shuffledDeck[NUM_CARDS];

    int i = 0;              // index for first pile
    int j = indexOfSplit;   // index for second pile
    int k = 0;              // index for shuffledDeck

    // Interleave cards while both piles have cards
    while (i < indexOfSplit && j < NUM_CARDS) {
        shuffledDeck[k++] = deckOfCards[i++];
        shuffledDeck[k++] = deckOfCards[j++];
    }

    // Copy remaining cards from first pile
    while (i < indexOfSplit) {
        shuffledDeck[k++] = deckOfCards[i++];
    }

    // Copy remaining cards from second pile
    while (j < NUM_CARDS) {
        shuffledDeck[k++] = deckOfCards[j++];
    }

    // Copy shuffled deck back into original deck
    for (int n = 0; n < NUM_CARDS; n++) {
        deckOfCards[n] = shuffledDeck[n];
    }
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
void updateColumns(LinkedList* columns, Card* deckOfCards[]) {
    // Remove cards if they exist
    for(int i = 0; i < NUM_COLUMNS; i++){
        while(columns[i].head != NULL) linked_list_pop(&columns[i]);
    }

    // Fill each column with a card
    for(int i = 0; i < NUM_CARDS; i++) {
        linked_list_push(&columns[i % NUM_COLUMNS], (void *)deckOfCards[i]);
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





