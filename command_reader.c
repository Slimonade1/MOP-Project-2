#include "command_reader.h"


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

        dealCards(deckOfCards, columns);
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
        dealCards(deckOfCards, columns);
        return GAME_STARTUP;
    }

    // SR (shuffles the deck to a random deck)
    if(strcmp(command, "SR") == 0) {
        strcpy(lastCommand, "SR");
        shuffleRandom(deckOfCards);
        dealCards(deckOfCards, columns);
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
        strcpy(lastCommand, "QQ");
        return GAME_QUIT;
    }

    if(strcmp(command, "P") == 0) {
        strcpy(lastCommand, "P");
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

GamePhase commandReaderPlay(
    LinkedList *columns,
    LinkedList *foundationCells,
    char *statusMessage,
    char *lastCommand) {

    char input[100];
    fgets(input, sizeof(input), stdin);

    // Remove newline
    input[strcspn(input, "\n")] = 0;

    strcpy(lastCommand, input);

    // Check for quit command
    if (strcmp(input, "QQ") == 0) {
        return GAME_QUIT;
    }

    Move move;
    if (!parseMove(input, &move)) {
        strcpy(statusMessage, "Invalid command format");
        return GAME_PLAY;
    }

    if (!validateMove(&move, columns, foundationCells)) {
        strcpy(statusMessage, "Illegal move");
        return GAME_PLAY;
    }

    // TODO: Execute the move
    executeMove(&move, columns, foundationCells);

    strcpy(statusMessage, "Move parsed successfully");
    return GAME_PLAY;
}

/**
 * Parses a move command string into a Move struct.
 *
 * Supported formats:
 * - C<source>:<card>->C<dest>  (e.g., C6:4H->C4)
 * - C<source>:<card>->F<dest> (e.g., C7:AS->F2)
 * - F<source>->C<dest>        (e.g., F4->C6)
 *
 * @param input The command string to parse
 * @param move Pointer to Move struct to populate
 * @return true if parsing succeeds, false otherwise
 */
bool parseMove(char *input, Move *move) {
    char temp[100];
    strcpy(temp, input);

    // Find positions of ':' and '->'
    char* colon = strchr(temp, ':');
    char* arrow = strstr(temp, "->");

    if (arrow == NULL) {
        return false; // No arrow found
    }

    // Split source part
    *arrow = '\0'; // Terminate source part
    char* source = temp;
    char* dest = arrow + 2; // Skip "->"

    // Parse source
    if (source[0] != 'C' && source[0] != 'F') {
        return false;
    }
    move->sourceType = source[0];
    move->sourceIndex = atoi(source + 1);

    // Check if card is specified (presence of ':')
    if (colon != NULL && colon < arrow) {
        // Card specified: C6:4H
        *colon = '\0'; // Terminate source
        char* cardStr = colon + 1;
        if (strlen(cardStr) != 2) {
            return false;
        }
        strcpy(move->card, cardStr);
    } else {
        // No card specified: F4
        move->card[0] = '\0';
    }

    // Parse destination
    if (dest[0] != 'C' && dest[0] != 'F') {
        return false;
    }
    move->destType = dest[0];
    move->destIndex = atoi(dest + 1);

    // Validate indices
    if (move->sourceType == 'C' && (move->sourceIndex < 1 || move->sourceIndex > 7)) {
        return false;
    }
    if (move->sourceType == 'F' && (move->sourceIndex < 1 || move->sourceIndex > 4)) {
        return false;
    }
    if (move->destType == 'C' && (move->destIndex < 1 || move->destIndex > 7)) {
        return false;
    }
    if (move->destType == 'F' && (move->destIndex < 1 || move->destIndex > 4)) {
        return false;
    }

    return true;
}

/**
 * validateMove - Validate a proposed move according to Yukon Solitaire rules.
 *
 * Rules enforced:
 * - Column → Column:
 *   - Source card(s) must exist in the source column.
 *   - The bottom card of the moved stack must be exactly one rank lower than the destination card.
 *   - Suits must differ (alternating color rule).
 *   - Empty destination columns accept only Kings.
 * - Column → Foundation:
 *   - Only the bottom (last) card of the source column may be moved.
 *   - Empty foundation accepts only Aces.
 *   - Non-empty foundation requires same suit and exactly one rank higher than the source card.
 * - Foundation → Column:
 *   - Only the top card of the source foundation may be moved.
 *   - Destination column requires source card to be exactly one rank lower and of a different suit.
 *   - Empty destination columns accept only Kings.
 *
 * @param move Pointer to the Move to validate (source/dest types, indices, card string).
 * @param columns Array of 7 `LinkedList` columns representing tableau columns.
 * @param foundationCells Array of 4 `LinkedList` foundation piles.
 * @return true if the move is legal under the rules above, false otherwise.
 */
bool validateMove(Move *move, LinkedList *columns, LinkedList *foundationCells){
    // Validate source
    Card* sourceCard = NULL;
    if (move->sourceType == 'C') {
        // Source is Column
        Node* current = columns[move->sourceIndex - 1].head;
        if(current == NULL) return false; // Source column is empty

        bool found = false;
        while(current && !found) {
            Card* card = (Card*)current->data;
            if(strcmp(card->data, move->card) == 0) found = true;
            current = current->next;
        }
        if(!found) return false; // Card was not found

        // For foundation moves, the source card must be the last card in the column
        if (move->destType == 'F' && current != NULL) return false;
        
    } else if (move->sourceType == 'F') {
        // Source is Foundation
        sourceCard = get_tail_card(&foundationCells[move->sourceIndex - 1]);
        if(sourceCard == NULL) return false; // Source foundation is empty
        
        // For foundation moves, card must match the bottom card
        if(strcmp(sourceCard->data, move->card) != 0) return false;
    }

    // Get destination card
    Card* destination = NULL;
    if (move->destType == 'C') {
        destination = get_tail_card(&columns[move->destIndex - 1]);
    } else if (move->destType == 'F') {
        destination = get_tail_card(&foundationCells[move->destIndex - 1]);
    }

    // Handle empty destination
    if (destination == NULL) {
        if (move->destType == 'C') {
            if (move->card[0] != 'K') return false;
        } else if (move->destType == 'F') {
            if (move->card[0] != 'A') return false;
        }
        return true;
    }

    // Check rank and suit based on move type
    int sourceRank = get_rank(move->card[0]);
    int destRank = get_rank(destination->data[0]);
    
    if (move->destType == 'C') {
        // Column-to-Column or Foundation-to-Column: source 1 lower, different suit
        if (sourceRank != destRank - 1) return false;
        if (move->card[1] == destination->data[1]) return false;  // Same suit invalid
    } else if (move->destType == 'F') {
        // Column-to-Foundation: source 1 higher, same suit
        if (sourceRank != destRank + 1) return false;
        if (move->card[1] != destination->data[1]) return false;  // Different suit invalid
    }

    return true;
}

int get_rank(char rank) {
    if (rank >= '2' && rank <= '9') return rank - '0';  // '2' -> 2, ..., '9' -> 9
    if (rank == 'T') return 10;
    if (rank == 'J') return 11;
    if (rank == 'Q') return 12;
    if (rank == 'K') return 13;
    if (rank == 'A') return 1;  // Ace is lowest
    return -1;  // Invalid rank
}

void executeMove(Move* move, LinkedList* columns, LinkedList* foundationCells) {
    LinkedList *src, *dst;

    // Resolve source
    src = (move->sourceType == 'C')
        ? &columns[move->sourceIndex - 1]
        : &foundationCells[move->sourceIndex - 1];

    // Resolve destination
    dst = (move->destType == 'C')
        ? &columns[move->destIndex - 1]
        : &foundationCells[move->destIndex - 1];

    /* === COLUMN -> COLUMN (STACK MOVE) === */
    if (move->sourceType == 'C' && move->destType == 'C') {
        Node *prev = NULL;
        Node *cur  = src->head;

        // Find start of stack
        while (cur) {
            Card *card = cur->data;
            if (strcmp(card->data, move->card) == 0) break;

            prev = cur;
            cur = cur->next;
        }

        // Detach from source
        if (prev) prev->next = NULL;
        else src->head = NULL;

        // Attach to destination
        if (!dst->head) {
            dst->head = cur;
        } else {
            Node *tail = dst->head;
            while (tail->next) tail = tail->next;
            tail->next = cur;
        }

        // Update sizes
        int moved = 0;
        Node *tmp = cur;
        while (tmp) {
            moved++;
            tmp = tmp->next;
        }
        src->size -= moved;
        dst->size += moved;
        return;
    }

    /* === COLUMN -> FOUNDATION (single card) === */
    if (move->sourceType == 'C' && move->destType == 'F') {
        Node *prev = NULL;
        Node *cur  = src->head;

        while (cur->next) {
            prev = cur;
            cur = cur->next;
        }

        if (prev)prev->next = NULL;
        else src->head = NULL;

        cur->next = NULL;
        src->size--;

        if (!dst->head)
            dst->head = cur;
        else {
            Node *tail = dst->head;
            while (tail->next) tail = tail->next;
            tail->next = cur;
        }

        dst->size++;
        return;
    }

    /* === FOUNDATION -> COLUMN (single card) === */
    if (move->sourceType == 'F' && move->destType == 'C') {
        Node *node = src->head;

        src->head = node->next;
        node->next = NULL;
        src->size--;

        if (!dst->head)
            dst->head = node;
        else {
            Node *t = dst->head;
            while (t->next) t = t->next;
            t->next = node;
        }

        dst->size++;
    }
}
