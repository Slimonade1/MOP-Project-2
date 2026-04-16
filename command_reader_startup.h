#ifndef COMMAND_READER_STARTUP_H
#define COMMAND_READER_STARTUP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command_reader_startup.h"
#include "linked_list.h"
#include "deck_loader.h"   // loadFile
#include "game.h"

#define DATA_DIR "data/"

GamePhase commandReaderStartup(
    LinkedList *deckOfCards,
    LinkedList *columns,
    char *statusMessage,
    char *lastCommand);

void showAllCards(LinkedList *deckOfCards);
void shuffleSplit(LinkedList *deckOfCards, int split);
void shuffleRandom(LinkedList *deckOfCards);
int saveFile(LinkedList *deckOfCards, char *fileName);

#endif // COMMAND_READER_STARTUP_H