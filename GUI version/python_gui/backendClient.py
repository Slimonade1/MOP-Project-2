import subprocess
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))           # Path to the directory containing main.py
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, ".."))    # One level up to the project root
BACKEND_DIR = os.path.join(PROJECT_ROOT, "c_backend")           # Path to the c_backend directory
BACKEND_PATH = os.path.join(BACKEND_DIR, "game.exe")            # Path to the compiled backend executable

proc = subprocess.Popen(
    BACKEND_PATH,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    text=True,
    cwd=BACKEND_DIR
)

def send(cmd):
    proc.stdin.write(cmd + "\n")
    proc.stdin.flush()

def read_state():
    lines = []
    while True:
        line = proc.stdout.readline()
        if not line:
            break
        if line.strip() == "BEGIN_STATE":
            break

    while True:
        line = proc.stdout.readline()
        if line.strip() == "END_STATE":
            break
        lines.append(line.rstrip())

    return lines

def parse_state(lines):
    columns = [[] for _ in range(7)]
    foundations = {"F1": [], "F2": [], "F3": [], "F4": []}
    COL_STARTS = [0, 8, 16, 24, 32, 40, 48]

    for line in lines:
        if not line.strip() or line.strip().startswith("C1"):
            continue

        line = line.expandtabs(8)

        for i, start in enumerate(COL_STARTS):
            raw = line[start:start+6]
            token = raw.strip()

            if token == "[]":
                columns[i].append("[]")          # skjult kort
            elif token == "":
                columns[i].append("")            # ingen kort
            else:
                columns[i].append(token)         # synligt kort

        parts = line.split()
        if len(parts) >= 2 and parts[-1] in foundations:
            card = parts[-2]
            if card != "[]":
                foundations[parts[-1]].append(card)


    return {"columns": columns, "foundations": foundations}
