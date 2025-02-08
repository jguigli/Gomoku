from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class Move(BaseModel):
    x: int
    y: int

# État du jeu (exemple simple)
board = [[0] * 19 for _ in range(19)]  # 19x19 plateau vide

@app.get("/board")
def get_board():
    return {"board": board}

@app.post("/move")
def make_move(move: Move):
    if board[move.x][move.y] == 0:
        board[move.x][move.y] = 1  # 1 = joueur, 2 = IA
        return {"message": "Move played", "board": board}
    return {"error": "Invalid move"}
