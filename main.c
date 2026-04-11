// Project 2 in Machine Oriented Programming
// This project is an implementation of the simple game Yukon Solitaire!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "card.h"

#define NUM_COLUMNS 7
#define NUM_CARDS 52

// Prototypes

int loadFile(Card* deckOfCards[], char* fileName);
void drawToTerminal(LinkedList* columns);
void populateColumnsWithCards(LinkedList* columns, Card* deckOfCards[]);
void populateColumnsWithBlanks(LinkedList* columns);
Card *get_card_at(LinkedList *list, int index);
static void trim_newline(char *line);
static int is_valid_card(const char *card);
static void free_deck(Card* deckOfCards[], int count);
void card_init(Card* card, char* data, bool visible);  // Add this


int main() {
    LinkedList columns[NUM_COLUMNS];
    Card* deckOfCards[NUM_CARDS] = { NULL };

    populateColumnsWithBlanks(columns);
    drawToTerminal(columns);

    // TODO Create a proper input reader that reads commands and executes, if they are valid

    char input[100];
    printf("INPUT >");
    scanf("%s", input);

    // this is a stupid way of doing this
    if(strcmp(input, "LD") == 0) {
        int loaded = loadFile(deckOfCards, "std_card_deck.txt");
        if (loaded != NUM_CARDS) {
            fprintf(stderr, "Failed to load full deck: loaded %d of %d cards\n", loaded, NUM_CARDS);
            free_deck(deckOfCards, loaded);
            return 1;
        }
    } else {
        int loaded = loadFile(deckOfCards, input);
        if (loaded != NUM_CARDS) {
            fprintf(stderr, "Failed to load full deck: loaded %d of %d cards\n", loaded, NUM_CARDS);
            free_deck(deckOfCards, loaded);
            return 1;
        }
    }

    populateColumnsWithCards(columns, deckOfCards);
    drawToTerminal(columns);

    //free_deck(deckOfCards, NUM_CARDS);
    return 0;
}

/**
 * Loads the deck of cards from the file "card_deck.txt" and saves
 * each valid card string into deckOfCards[].
 * Returns the number of cards successfully loaded.
 */
int loadFile(Card* deckOfCards[], char* fileName){
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    int index = 0;
    while (index < NUM_CARDS && fgets(buffer, sizeof(buffer), file) != NULL) {
        trim_newline(buffer);
        if (buffer[0] == '\0') {
            continue; // skip empty lines
        }

        if (!is_valid_card(buffer)) {
            fprintf(stderr, "%s is NOT a valid card\n", buffer);
            return 0;
        }

        Card* newCard = malloc(sizeof(Card));
        if (newCard == NULL) {
            perror("Memory allocation failed");
            break;
        }
        card_init(newCard, buffer, true);
        deckOfCards[index] = newCard;

        index++;
    }

    fclose(file);
    return index;
}

static void trim_newline(char *line) {
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[--len] = '\0';
    }
}

static int is_valid_card(const char *card) {
    // Checks if length of cards is 2
    if (card == NULL || card[0] == '\0' || card[1] == '\0' || card[2] != '\0') {
        return 0;
    }

    const char *valid_ranks = "A23456789TJQK";
    const char *valid_suits = "HDCS";
    return strchr(valid_ranks, card[0]) != NULL && strchr(valid_suits, card[1]) != NULL;
}

static void free_deck(Card* deckOfCards[], int count) {
    for (int i = 0; i < count; i++) {
        if (deckOfCards[i]) {
            free(deckOfCards[i]->data);
            free(deckOfCards[i]);
        }
    }
}

/**
 * Draws the GUI of the game to the console
 */
void drawToTerminal(LinkedList* columns) {
    // Column headers
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
            Card *card = get_card_at(&columns[col], row);
            char *display = card ? (card->shown ? card->data : "[]") : "";
            printf("%-2s\t", display);
        }
        printf("\n");
    }
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
 * Initialize coloumns and populate with cards
 */
void populateColumnsWithCards(LinkedList* columns, Card* deckOfCards[]) {
    // Initialize all columns
    for(int i = 0; i < NUM_COLUMNS; i++) {
        linked_list_init(&columns[i]);
    }

    // Fill each column with a card
    for(int i = 0; i < NUM_CARDS; i++) {
        linked_list_push(&columns[i % NUM_COLUMNS], (void *)deckOfCards[i]);
    }
}

/**
 * Initialize coloumns and populate with hidden cards
 */
void populateColumnsWithBlanks(LinkedList* columns) {
    // Initialize all columns
    for(int i = 0; i < NUM_COLUMNS; i++) {
        linked_list_init(&columns[i]);
    }

    // Fill each column with a card
    Card* blankCard = malloc(sizeof(Card));
    if (blankCard == NULL) {
        perror("Memory allocation failed");
        return;
    }
    card_init(blankCard, "", false);
    for(int i = 0; i < NUM_CARDS; i++) {
        linked_list_push(&columns[i % NUM_COLUMNS], blankCard);
    }
}





