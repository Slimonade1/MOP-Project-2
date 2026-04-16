// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "linked_list.h"
#include "deck_loader.h"
#include "card.h"

// Global variables
#define NUM_COLUMNS 7
#define NUM_CARDS 52
#define DATA_DIR "data/"

char statusMessage[50];
char lastCommand[10];
typedef enum {
    GAME_STARTUP,
    GAME_PLAY,
    GAME_QUIT
} GamePhase;

LinkedList columns[NUM_COLUMNS];
LinkedList finishCells[4];

// Prototypes

GamePhase commandReaderStartup(LinkedList *deckOfCards);
void showAllCards(LinkedList *deckOfCards);
void shuffleSplit(LinkedList *deckOfCards, int split);
void shuffleRandom(LinkedList *deckOfCards);
int saveFile(LinkedList *deckOfCards, char *fileName);

void drawToTerminal(LinkedList* columns, LinkedList* finishCells);
Card *get_card_at(LinkedList *list, int index);

void initializeColumns(LinkedList* columns, LinkedList* finishCells);
void updateColumns(LinkedList *deck, LinkedList *columns);
void freeDeck(LinkedList *deckOfCards);

int main() {
    LinkedList deckOfCards;
    linked_list_init(&deckOfCards);

    GamePhase phase = GAME_STARTUP;

    initializeColumns(columns, finishCells);
    srand((unsigned)time(NULL)); // create new seed

    while(phase != GAME_QUIT) {
        drawToTerminal(columns, finishCells); // Empty grid start of game

        switch(phase) {
            case GAME_STARTUP:
                phase = commandReaderStartup(&deckOfCards);
                break;
            
            case GAME_PLAY:
                // TODO IMPLEMENT THIS
                phase = GAME_QUIT;
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
 *  Reads commands from the terminal
 * 
 *  ... TODO write more documentation when function is finished
 */
GamePhase commandReaderStartup(LinkedList *deckOfCards) {
    // STARTUP phase
    char input[100];
    fgets(input, sizeof(input), stdin);

    // Tokenize the input into command and argument
    char *command = strtok(input, " \t\n");
    char *argument = strtok(NULL, " \t\n");

    char path[256]; // Path for loadFile and saveFile

    // LD <filename> (loads deck into active deck)
    if(strcmp(command, "LD") == 0) { 
        strcpy(lastCommand, "LD");

        freeDeck(deckOfCards);
        linked_list_init(deckOfCards);

        snprintf(path, sizeof(path), "%s%s", DATA_DIR, argument); // insert data directory

        int loaded = loadFile(deckOfCards, path);
        if (loaded != NUM_CARDS) {
            strcpy(statusMessage, "Failed to load deck");
            freeDeck(deckOfCards);
            return GAME_STARTUP;
        }

        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SW (shows contents of deck)
    if(strcmp(command, "SW") == 0) {
        strcpy(lastCommand, "SW");
        showAllCards(deckOfCards);
        return GAME_STARTUP;
    }

    // SI <split> (shuffles the deck from a given index)
    if(strcmp(command, "SI") == 0) {
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
                return GAME_STARTUP;
            }

            if(split <= 0 || split >= NUM_CARDS) {
                strcpy(statusMessage, "Index out of bounds");
                return GAME_STARTUP;
            }
        }

        shuffleSplit(deckOfCards, split);
        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SR (shuffles the deck to a random deck)
    if(strcmp(command, "SR") == 0) {
        strcpy(lastCommand, "SR");
        shuffleRandom(deckOfCards);
        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SD <filename> (saves the current deck to <filename>)
    if(strcmp(command, "SD") == 0) {
        strcpy(lastCommand, "SD");
        if(argument == NULL){
            strcpy(statusMessage, "Missing filename");
        }

        snprintf(path, sizeof(path), "%s%s", DATA_DIR, argument); // insert data directory

        int saved = saveFile(deckOfCards, path);
        if(saved != 0){
            strcpy(statusMessage, "Failed to save deck");
            return GAME_STARTUP;
        }
    }

    if(strcmp(command, "QQ") == 0) {
        return GAME_QUIT;
    }

    if(strcmp(command, "P") == 0) {
        return GAME_PLAY;
    }

    strcpy(statusMessage, "Unknown command");
    return GAME_STARTUP;
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

int saveFile(LinkedList *deckOfCards, char *fileName){    
    if(strcmp(fileName, "data/std_card_deck.txt") == 0) return 1;   // Guard against changing the standard deck
    
    FILE *filePrinter;

    // Open a file in write mode
    filePrinter = fopen(fileName, "w");
    if(!filePrinter) return 1;

    Node *current = deckOfCards->head;
    while(current) {
        Card *card = (Card *)current->data;
        fprintf(filePrinter, "%s\n", card->data);
        current = current->next;
    }

    fclose(filePrinter);
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




