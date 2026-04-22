# Yukon Solitaire – MOP Project 2

This project is an implementation of **Yukon Solitaire** written in **C**, developed as part of the *02322 Machine Oriented Programming* course.

The program is terminal-based and demonstrates low-level programming concepts such as dynamic memory management, linked lists, modular design, and explicit state handling.

---

## Features

- Text-based Yukon Solitaire game
- Linked-list based card and pile representation (no arrays for card storage)
- Multiple game phases:
  - **STARTUP** – load, shuffle, and configure the deck
  - **PLAY** – perform legal solitaire moves
  - **QUIT** – clean program termination
- Command-driven interface
- Deck loading and saving to files
- Input validation and rule enforcement

---

## Compilation

Compile the program using `gcc`:

```bash
gcc *.c -o game
```

This will produce an executable named `game`.

---

## Running the Game

Run the program from the project directory:

```bash
./game
```

The game reads commands from standard input and displays the board state after each command.

---

## Game Phases

### STARTUP Phase
In this phase, the player can issue setup commands **without restriction**.

Supported commands:
- `LD <filename>` — Load a deck from file
- `SI <n>` — Shuffle deck using split-interleave at index `n`
- `SR` — Random shuffle
- `SW` — Show all cards
- `SD <filename>` — Save current deck
- `P` — Transition to PLAY phase
- `QQ` — Quit game

---

### PLAY Phase
In this phase, the player performs legal Yukon Solitaire moves.

Supported move formats:
- `C<source>:<card>->C<dest>`
- `C<source>:<card>->F<dest>`
- `F<source>->C<dest>`
- `QQ` — Quit game

All moves are parsed, validated, and executed according to Yukon rules.

---

## Project Structure

```
.
├── main.c               # Program entry point and main game loop
├── command_reader.c     # Input parsing and command handling
├── command_reader.h
├── game.h               # Shared game state and core definitions
├── linked_list.c        # Generic linked list implementation
├── linked_list.h
├── deck_loader.c        # File loading logic
├── deck_loader.h
├── card.c               # Card representation and helpers
├── card.h
├── data/                # Contains card decks
└── README.md
```

---

## Data Structures

### Card Representation
Each card is represented as a dynamically allocated `Card` structure containing:
- Rank and suit
- Visibility state

### Linked Lists
All card piles (deck, columns, foundation piles) are implemented using a **singly linked list** structure.
Nodes are owned by their list; game logic performs pointer-level list manipulation where required.

### Game State
The game uses an explicit **finite-state model** with clearly defined transitions between phases.

---

## Design Notes

- No arrays are used for storing cards or piles.
- All memory allocation and cleanup is explicit.
- Node ownership and list invariants are carefully preserved.
- Parsing, validation, and execution are strictly separated.

---

## Limitations

- Terminal UI only
- No undo functionality
- No win detection (yet)

---

## Author

Simon Toftemark  
02322 Machine Oriented Programming  
DTU (Technical University of Denmark)

---

## License

This project was developed for educational purposes as part of a university course.
