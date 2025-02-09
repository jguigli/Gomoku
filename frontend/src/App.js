import React, { useState, useEffect } from "react";
import axios from "axios";
import "./App.css";

const BOARD_SIZE = 19;

const App = () => {
  const [board, setBoard] = useState(Array(BOARD_SIZE).fill(Array(BOARD_SIZE).fill(0)));
  const [turn, setTurn] = useState(1);

  useEffect(() => {
    fetchBoard();
  }, []);

  // Fonction pour récupérer le plateau depuis l'API Python
  const fetchBoard = async () => {
    try {
      const response = await axios.get("http://localhost:8000/board");
      setBoard(response.data.board);
    } catch (error) {
      console.error("Erreur lors de la récupération du plateau:", error);
    }
  };

  // Fonction pour jouer un coup
  const playMove = async (x, y) => {
    try {
      const response = await axios.post("http://localhost:8000/move", { x, y });
      setBoard(response.data.board);
      setTurn(turn === 1 ? 2 : 1); // Alterner les joueurs
    } catch (error) {
      console.error("Erreur lors du placement du pion:", error);
    }
  };

  return (
    <div className="container">
      <h1>Gomoku</h1>
      <div className="board">
        {board.map((row, rowIndex) =>
          row.map((cell, colIndex) => (
            <div
              key={`${rowIndex}-${colIndex}`}
              className={`cell ${cell === 1 ? "black" : cell === 2 ? "white" : ""}`}
              onClick={() => playMove(colIndex, rowIndex)}
            />
          ))
        )}
      </div>
      <p>Tour du joueur : {turn === 1 ? "⚫ Noir" : "⚪ Blanc"}</p>
    </div>
  );
};

export default App;

