#ifndef COMMAND_READER_H
#define COMMAND_READER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"
#include "deck_loader.h"
#include "game.h"

#define DATA_DIR "data/"

// Startup phase

GamePhase commandReaderStartup(
    LinkedList *deckOfCards,
    LinkedList *columns,
    char *statusMessage,
    char *lastCommand
);

void showAllCards(LinkedList *deckOfCards);
void shuffleSplit(LinkedList *deckOfCards, int split);
void shuffleRandom(LinkedList *deckOfCards);
int saveFile(LinkedList *deckOfCards, char *fileName);

// Play Phase

GamePhase commandReaderPlay(
    LinkedList *columns,
    LinkedList *foundationCells,
    char *statusMessage,
    char *lastCommand
);

typedef struct {
    char sourceType; // 'C' for column, 'F' for foundation
    int sourceIndex; // 1-7 for columns, 1-4 for foundations
    char card[3];    // "4H", "AS", etc. (empty if not specified)
    char destType;   // 'C' or 'F'
    int destIndex;   // 1-7 or 1-4
} Move;

bool parseMove(char* input, Move* move);
bool validateMove(Move* move, LinkedList *columns, LinkedList *foundationCells);
int get_rank(char rank);
void executeMove(Move* move, LinkedList* columns, LinkedList* foundationCells);

#endif // COMMAND_READER_H