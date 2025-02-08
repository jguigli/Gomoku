import tkinter as tk

BOARD_SIZE = 19
CELL_SIZE = 30

class GomokuApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Gomoku avec Tkinter")

        self.board = [[0] * BOARD_SIZE for _ in range(BOARD_SIZE)]  # Plateau vide
        self.turn = 1  # 1 = Joueur 1, 2 = Joueur 2

        self.canvas = tk.Canvas(self.root, width=BOARD_SIZE * CELL_SIZE, height=BOARD_SIZE * CELL_SIZE, bg="beige")
        self.canvas.pack()
        self.canvas.bind("<Button-1>", self.click_board)

        self.draw_board()

    def draw_board(self):
        """Dessine la grille du plateau"""
        for i in range(BOARD_SIZE):
            self.canvas.create_line(i * CELL_SIZE, 0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE, fill="black")
            self.canvas.create_line(0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE, fill="black")

    def click_board(self, event):
        """Gère le clic du joueur pour placer une pierre"""
        x, y = event.x // CELL_SIZE, event.y // CELL_SIZE

        if self.board[y][x] == 0:  # Vérifie si la case est vide
            self.board[y][x] = self.turn  # Place la pierre
            self.draw_stone(x, y, self.turn)

            # Change de joueur
            self.turn = 3 - self.turn  # Alterne entre 1 et 2

    def draw_stone(self, x, y, player):
        """Dessine une pierre sur le plateau"""
        color = "black" if player == 1 else "white"
        self.canvas.create_oval(
            x * CELL_SIZE + 5, y * CELL_SIZE + 5,
            (x + 1) * CELL_SIZE - 5, (y + 1) * CELL_SIZE - 5,
            fill=color, outline="black"
        )

if __name__ == "__main__":
    root = tk.Tk()
    app = GomokuApp(root)
    root.mainloop()
