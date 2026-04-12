#ifndef DECK_LOADER_H
#include "card.h"

int loadFile(Card* deckOfCards[], char* fileName);
static void trim_newline(char *line);
static int is_valid_card(const char *card);


#endif // DECK_LOADER_H