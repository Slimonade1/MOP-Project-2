#ifndef DECK_LOADER_H
#include "linked_list.h"
#include "card.h"

int loadFile(LinkedList *deckOfCards, char* fileName);
static void trim_newline(char *line);
static int is_valid_card(const char *card);


#endif // DECK_LOADER_H