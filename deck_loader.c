#include "deck_loader.h"
#include "linked_list.h"
#include "card.h"
#include <stdio.h>
#include <string.h>

#define NUM_CARDS 52
#define DATA_DIR "data/"

/**
 * Loads the deck of cards from the file "card_deck.txt" and saves
 * each valid card string into deckOfCards[].
 * Returns the number of cards successfully loaded.
 */
int loadFile(LinkedList *deckOfCards, char* fileName){
    FILE *file = NULL;

    // If no filename is provided, use the default deck file
    if(strcmp(fileName, DATA_DIR)) file = fopen("data/std_card_deck.txt", "r");
    else file = fopen(fileName, "r");

    if (file == NULL) {
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
        card_init(newCard, buffer, false);
        linked_list_push(deckOfCards, newCard);

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