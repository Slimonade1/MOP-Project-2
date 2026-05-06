#include "card.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void card_init(Card* card, char* data, bool shown){
    card->data = malloc(strlen(data) + 1);
    if (card->data) {
        strcpy(card->data, data);
    }
    card->shown = shown;
}

// DEPRECATED
void card_change_visibility(Card* card){
    card->shown = !card->shown;
}