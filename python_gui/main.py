import backendClient
import tkinter as tk

CARD_W, CARD_H = 70, 100
ROW_SPACING = 40
COL_X = [50 + i * 110 for i in range(7)]
ROW_Y_START = 40

FOUND_X = 900
FOUND_Y_START = 40
FOUND_SPACING = 120

root = tk.Tk()
root.title("Yukon")
canvas = tk.Canvas(root, width=1200, height=700, bg="darkgreen")
canvas.pack()

entry = tk.Entry(root, font=("Consolas", 14))
entry.pack(side="bottom", fill="x")

# Store mappings from canvas item ID -> column index and card text
canvas.card_pos = {}
canvas.card_text = {}

def draw_cards(canvas, columns):
    """Draw all tableau columns on the canvas."""
    canvas.delete("all")
    canvas.card_pos.clear()
    canvas.card_text.clear()

    for col_idx, col in enumerate(columns):
        x = COL_X[col_idx]
        y_stack = 0

        for card in col:
            if card == "":
                continue

            y = ROW_Y_START + y_stack * ROW_SPACING
            y_stack += 1

            if card == "[]":
                # Face-down card
                canvas.create_rectangle(x, y, x + CARD_W, y + CARD_H, fill="gray", outline="black", tags="card")
            else:
                # Face-up card
                color = "red" if card[-1] in "HD" else "black"

                rect_id = canvas.create_rectangle(x, y, x + CARD_W, y + CARD_H, fill="white", outline="black", tags="card")
                text_id = canvas.create_text(x + CARD_W // 2, y + 15, text=card, font=("Arial", 16, "bold"), fill=color, anchor="n", tags="card")

                # Map both rect and text IDs to column and card
                canvas.card_pos[rect_id] = col_idx
                canvas.card_pos[text_id] = col_idx
                canvas.card_text[rect_id] = card
                canvas.card_text[text_id] = card

    # Redraw foundations on top
    draw_foundations(canvas, current_state.get("foundations", {}))


def draw_foundations(canvas, foundations):
    """Draw the four foundation piles."""
    for i, f in enumerate(["F1", "F2", "F3", "F4"]):
        x = FOUND_X
        y = FOUND_Y_START + i * FOUND_SPACING

        # Empty foundation outline
        canvas.create_rectangle(x, y, x + CARD_W, y + CARD_H, outline="white", width=2, tags="foundation")

        # Draw top card if foundation is not empty
        if foundations.get(f):
            card = foundations[f][-1]
            color = "red" if card[-1] in "HD" else "black"
            canvas.create_rectangle(x, y, x + CARD_W, y + CARD_H, fill="white", outline="black", tags="foundation")
            canvas.create_text(x + CARD_W // 2, y + 15, text=card, font=("Arial", 16, "bold"), fill=color, anchor="n", tags="foundation")

def send_command(event=None):
    cmd = entry.get()
    entry.delete(0, tk.END)

    backendClient.send(cmd)
    state = backendClient.parse_state(backendClient.read_state())
    draw_cards(canvas, state["columns"])
    draw_foundations(canvas, state["foundations"])

entry.bind("<Return>", send_command)

# Start game and initial draw
current_state = backendClient.parse_state(backendClient.read_state())
draw_cards(canvas, current_state["columns"])

root.mainloop()
