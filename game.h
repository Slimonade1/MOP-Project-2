#ifndef GAME_H
#define GAME_H

#include "linked_list.h"

/* Game phases */
typedef enum {
    GAME_STARTUP,
    GAME_PLAY,
    GAME_QUIT
} GamePhase;

/* Constants */
#define NUM_COLUMNS 7
#define NUM_CARDS 52

/* Global game state */
extern LinkedList columns[NUM_COLUMNS];
extern LinkedList finishCells[4];

extern char statusMessage[50];
extern char lastCommand[10];

/* Core game functions */
void updateColumns(LinkedList *deckOfCards, LinkedList *columns);
void freeDeck(LinkedList *deckOfCards);

#endif