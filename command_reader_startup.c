#include "command_reader_startup.h"


/**
 * Reads and executes a single command during the STARTUP phase of the game.
 *
 * This function reads input from standard input, parses a command and an
 * optional argument, and performs setup-related actions such as loading a deck,
 * shuffling, revealing cards, or saving the deck to a file.
 *
 * Supported commands in the STARTUP phase include:
 *   -LD <filename> : Load a deck from a file
 *   -SW            : Reveal all cards in the deck
 *   -SI <split>    : Shuffle the deck by interleaving at a given index
 *   -SR            : Shuffle the deck randomly
 *   -SD <filename> : Save the current deck to a file
 *   -P             : Transition to the PLAY phase
 *   -QQ            : Quit the game\n
 *
 * Error conditions (invalid commands, invalid arguments, or failed file
 * operations) are reported via the global statusMessage and do not
 * terminate the STARTUP phase.
 *
 * @param deckOfCards Pointer to the linked list representing the current deck
 *                    of cards.
 * @return The next GamePhase, which is GAME_STARTUP for normal
 *         execution, GAME_PLAY when starting the game, or
 *         GAME_QUIT when quitting.
 */
GamePhase commandReaderStartup(
    LinkedList *deckOfCards,
    LinkedList *columns,
    char *statusMessage,
    char *lastCommand) {

    // STARTUP phase
    char input[100];
    fgets(input, sizeof(input), stdin);

    // Tokenize the input into command and argument
    char *command = strtok(input, " \t\n");
    char *argument = strtok(NULL, " \t\n");

    char path[256]; // Path for loadFile and saveFile

    // LD <filename> (loads deck into active deck)
    if(strcmp(command, "LD") == 0) { 
        strcpy(lastCommand, "LD");

        freeDeck(deckOfCards);
        linked_list_init(deckOfCards);

        snprintf(path, sizeof(path), "%s%s", DATA_DIR, argument); // insert data directory

        int loaded = loadFile(deckOfCards, path);
        if (loaded != NUM_CARDS) {
            strcpy(statusMessage, "Failed to load deck");
            freeDeck(deckOfCards);
            return GAME_STARTUP;
        }

        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SW (shows contents of deck)
    if(strcmp(command, "SW") == 0) {
        strcpy(lastCommand, "SW");
        showAllCards(deckOfCards);
        return GAME_STARTUP;
    }

    // SI <split> (shuffles the deck from a given index)
    if(strcmp(command, "SI") == 0) {
        strcpy(lastCommand, "SI");
            
        int split;

        if(argument == NULL) {
            // If no argument was given, split at random position
            split = rand() % (NUM_CARDS - 1) + 1; // 1 .. NUM_CARDS-1
        } else {
            char *end;
            split = (int)strtol(argument, &end, 10);
            if (end == argument || *end != '\0') {
                strcpy(statusMessage, "Argument must be an int");
                return GAME_STARTUP;
            }

            if(split <= 0 || split >= NUM_CARDS) {
                strcpy(statusMessage, "Index out of bounds");
                return GAME_STARTUP;
            }
        }

        shuffleSplit(deckOfCards, split);
        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SR (shuffles the deck to a random deck)
    if(strcmp(command, "SR") == 0) {
        strcpy(lastCommand, "SR");
        shuffleRandom(deckOfCards);
        updateColumns(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SD <filename> (saves the current deck to <filename>)
    if(strcmp(command, "SD") == 0) {
        strcpy(lastCommand, "SD");
        if(argument == NULL){
            strcpy(statusMessage, "Missing filename");
            return GAME_STARTUP;
        }

        snprintf(path, sizeof(path), "%s%s", DATA_DIR, argument); // insert data directory

        int saved = saveFile(deckOfCards, path);
        if(saved != 0){
            strcpy(statusMessage, "Failed to save deck");
            return GAME_STARTUP;
        }
    }

    if(strcmp(command, "QQ") == 0) {
        return GAME_QUIT;
    }

    if(strcmp(command, "P") == 0) {
        return GAME_PLAY;
    }

    strcpy(statusMessage, "Unknown command");
    return GAME_STARTUP;
}



/**
 * Shuffles a deck of cards in place using random insertion on a linked list.
 *
 * @param deckOfCards Pointer to the linked-list deck to be shuffled.
 */
void shuffleRandom(LinkedList *deckOfCards) {
    LinkedList shuffled;
    linked_list_init(&shuffled);

    while (deckOfCards->head) {
        // Remove card from top of deck
        void *card = linked_list_pop_front(deckOfCards);

        // Choose random insertion position
        int pos = rand() % (shuffled.size + 1);

        // Insert card data into shuffled deck
        linked_list_insert_at(&shuffled, card, pos);
    }

    *deckOfCards = shuffled;
}

/**
 * Changes all cards to be shown in the UI
 */
void showAllCards(LinkedList *deckOfCards) {
    Node *current = deckOfCards->head;

    while(current) {
        Card *card = current->data;
        card->shown = true;
        current = current->next;
    }
}

/**
 * Shuffles the deck by splitting it into two piles and interleaving their cards.
 *
 * The deck is split at the given index and cards are alternately taken from each
 * pile to form a new shuffled deck.
 *
 * @param deckOfCards LinkedList of {@code NUM_CARDS} length with cards
 * @param split Index at which the deck is split (0 < split < NUM_CARDS).
 */
void shuffleSplit(LinkedList *deckOfCards, int split) {
    LinkedList a, b, shuffled;
    linked_list_init(&a);
    linked_list_init(&b);
    linked_list_init(&shuffled);

    // Split the deck into two piles
    for (int i = 0; i < split; i++) {
        linked_list_push(&a, linked_list_pop_front(deckOfCards));
    }

    while (deckOfCards->head) {
        linked_list_push(&b, linked_list_pop_front(deckOfCards));
    }

    // Interleave: take from A then B
    while (a.head && b.head) {
        linked_list_push(&shuffled, linked_list_pop_front(&a));
        linked_list_push(&shuffled, linked_list_pop_front(&b));
    }

    // Append remainder
    while (a.head) linked_list_push(&shuffled, linked_list_pop_front(&a));
    while (b.head) linked_list_push(&shuffled, linked_list_pop_front(&b));

    *deckOfCards = shuffled;

}

int saveFile(LinkedList *deckOfCards, char *fileName){    
    if(strcmp(fileName, "data/std_card_deck.txt") == 0) return 1;   // Guard against changing the standard deck
    
    FILE *filePrinter;

    // Open a file in write mode
    filePrinter = fopen(fileName, "w");
    if(!filePrinter) return 1;

    Node *current = deckOfCards->head;
    while(current) {
        Card *card = (Card *)current->data;
        fprintf(filePrinter, "%s\n", card->data);
        current = current->next;
    }

    fclose(filePrinter);
    return 0;
}