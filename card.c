#include "card.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void card_init(Card* card, char* data, bool* shown){
    card->data = data;
    card->shown = true;
}

void card_change_visibility(Card* card){
    card->shown = !card->shown;
}