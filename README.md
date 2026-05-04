# Yukon Solitaire — GUI Version

This README describes the GUI variant of the Yukon Solitaire project located in the GUI version folder. The GUI is a small Python front-end (Tkinter) that talks to the C backend executable.

**Contents**

- `GUI version/c_backend/` — C backend (same core game logic as the console version). Produces `game.exe` on Windows.
- `GUI version/python_gui/` — Python GUI client (`main.py`) and `backendClient.py` which spawns the C backend and exchanges commands/state.

---

**Prerequisites (Windows)**

- Python 3.8+ (Tkinter is required; included with standard CPython installers)
- A C compiler that can produce a Windows executable (e.g., MinGW-w64 or MSYS2 `gcc`).

---

Building the backend (c_backend)

1. Open a terminal and change to the `GUI version\c_backend` directory.
2. Compile with `gcc` (MinGW/MSYS):

```bash
gcc *.c -O2 -o game.exe
```

This should produce `game.exe` inside `GUI version/c_backend`. The Python client expects the executable to be named `game.exe`.

---

Running the GUI

1. Make sure `game.exe` exists in `GUI version/c_backend`.
2. From the `GUI version/python_gui` directory run:

```bash
python main.py
```

3. The Tkinter window will open. Use the text entry at the bottom to type the same commands used in the console version (e.g., `LD <deckfile>`, `P`, moves, `QQ`). Press Enter to send commands.

Notes:
- The GUI uses `backendClient.py` to launch `game.exe` and parse the textual state output between `BEGIN_STATE` / `END_STATE` markers.
- Deck files are located in the `data/` directories; you can pass relative paths from the `c_backend` working directory (or copy desired decks into `GUI version/c_backend/data/`).

---

Implementation details

- The GUI front-end is implemented using `tkinter` (`main.py`).
- `backendClient.py` launches the backend and provides `send()`, `read_state()` and `parse_state()` helpers.

---

Troubleshooting

- If the GUI fails to start because `game.exe` cannot be found, confirm compilation output and that the executable is named `game.exe`.
- If `tkinter` is missing, install the standard Python distribution that includes Tk support or install the appropriate OS packages.

---

Author

Simon Toftemark
