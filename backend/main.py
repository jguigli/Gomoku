from fastapi import FastAPI
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # http://localhost:3000
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class Move(BaseModel):
    x: int
    y: int

BOARD_SIZE = 19
board = [[0] * BOARD_SIZE for _ in range(BOARD_SIZE)]
turn = 1  # 1 = Noir, 2 = Blanc

@app.get("/board")
def get_board():
    return {"board": board}

@app.post("/move")
def make_move(move: Move):
    global turn
    if board[move.y][move.x] == 0:
        board[move.y][move.x] = turn
        turn = 3 - turn  # Alterner le tour
        return {"message": "Coup joué", "board": board}
    return {"error": "Coup invalide"}
