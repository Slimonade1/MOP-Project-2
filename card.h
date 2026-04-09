#ifndef CARD_H
#define CARD_H

#include <stdlib.h>
#include <stdbool.h>

// Card structure
typedef struct Card {
    char* data;
    bool shown;
} Card;

#endif // CARD_H