#include "deck_loader.h"
#include "linked_list.h"
#include "card.h"
#include "game.h"
#include <stdio.h>
#include <string.h>

#define NUM_CARDS 52
#define DATA_DIR "data/"

/**
 * Loads the deck of cards from the file "card_deck.txt" and saves
 * each valid card string into deckOfCards.
 */
int loadFile(LinkedList *deckOfCards, char* fileName){
    FILE *file = NULL;

    file = fopen(fileName, "r");
    char errorMsg[100];

    if (file == NULL) {
        snprintf(errorMsg, sizeof(errorMsg), "Failed to open deck file: %s", fileName);
        strcpy(statusMessage, errorMsg);
        return 1;
    }

    char buffer[256];
    int index = 0;

    while (index < NUM_CARDS && fgets(buffer, sizeof(buffer), file) != NULL) {
        trim_newline(buffer);
        if (buffer[0] == '\0') {
            continue; // skip empty lines
        }

        if (!is_valid_card(buffer)) {
            snprintf(errorMsg, sizeof(errorMsg), "Invalid card found in deck file: %s", buffer);
            strcpy(statusMessage, errorMsg);

            return 1;
        }

        // Check if card is already in the deck
        if (is_duplicate_card(deckOfCards, buffer)) {
            snprintf(errorMsg, sizeof(errorMsg), "Duplicate card found in deck file: %s", buffer);
            strcpy(statusMessage, errorMsg);
            return 1;
        }

        Card* newCard = malloc(sizeof(Card));
        if (newCard == NULL) {
            perror("Memory allocation failed");
            break;
        }
        card_init(newCard, buffer, false);
        linked_list_push(deckOfCards, newCard);

        index++;
    }

    if(index < NUM_CARDS) {
        snprintf(errorMsg, sizeof(errorMsg), "Not enough valid cards in deck file: %d found, %d expected", index, NUM_CARDS);
        strcpy(statusMessage, errorMsg);
        return 1;
    }

    fclose(file);
    return 0;
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

static int is_duplicate_card(LinkedList *deckOfCards, const char *card) {
    Node *current = deckOfCards->head;
    while (current) {
        Card *existingCard = (Card *)current->data;
        if (strcmp(existingCard->data, card) == 0) {
            return 1; // Duplicate found
        }
        current = current->next;
    }
    return 0; // No duplicate
}