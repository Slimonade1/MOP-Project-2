#ifndef CARD_H
#define CARD_H

#include <stdlib.h>
#include <stdbool.h>

// Card structure
typedef struct Card {
    char* data;
    bool shown;
} Card;

// Function prototypes
void card_init(Card* card, char* data, bool shown);
void card_change_visibility(Card* card);

#endif // CARD_H