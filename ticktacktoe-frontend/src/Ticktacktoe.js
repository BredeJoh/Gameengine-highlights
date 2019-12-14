import React from "react";
import O from "./o.png";
import X from "./x.png";

function Square(props) {
  return (
    <div className="square" onClick={props.onClick}>
      {props.value}
    </div>
  );
}

class Board extends React.Component {
  renderSquare(i) {
    return (
      <Square
        value={this.props.squares[i]}
        onClick={() => this.props.onClick(i)}
      />
    );
  }

  render() {
    return (
      <div>
        <div className="board-row">
          {this.renderSquare(0)}
          {this.renderSquare(1)}
          {this.renderSquare(2)}
        </div>
        <div className="board-row">
          {this.renderSquare(3)}
          {this.renderSquare(4)}
          {this.renderSquare(5)}
        </div>
        <div className="board-row">
          {this.renderSquare(6)}
          {this.renderSquare(7)}
          {this.renderSquare(8)}
        </div>
      </div>
    );
  }
}

class Game extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      history: [
        {
          squares: Array(9).fill(null),
          images: Array(9).fill(null),
        }
      ],
      stepNumber: 0,
      player: this.props.isHosting() ? "X" : "O",
      room: this.props.getRoom() + "1",
      currentPlayer: "X"
    };
    this.setupEndPoints(this.props.socket);
    setTimeout(() => {
      this.props.socket.emit("joinRoom", this.state.room);
    }, 250);
  }

  setupEndPoints(socket) {
    socket.on("leaveGame", room => {
      this.setState({
        history: [
          {
            squares: Array(9).fill(null),
            images: Array(9).fill(null),
          }
        ],
        stepNumber: 0,
        player: "X",
        room: "",
        currentPlayer: "X",
        gameOver: false
      });
      window.location.href = window.location.href.split("?")[0];
    });

    socket.on("makeMove", (index, player) => {
      const history = this.state.history.slice(0, this.state.stepNumber + 1);
      const current = history[history.length - 1];
      const squares = current.squares.slice();
      const images = current.images.slice();

      let img;

      if (player === "X") {
        img = <img className="squareImgX" src={X} alt="logo.svg" />;
      } else {
        img = <img className="squareImgO" src={O} alt="logo.svg" />;
      }

      images[index] = img;
      squares[index] = player;
      let nextPlayer = "X";
      if (this.state.currentPlayer === "X") nextPlayer = "O";
      this.setState({
        history: history.concat([
          {
            squares: squares,
            images: images
          }
        ]),
        stepNumber: history.length,
        yourTurn: !this.state.yourTurn,
        currentPlayer: nextPlayer
      });

      const newHistory = this.state.history;
      const newCurrent = newHistory[this.state.stepNumber];
      const winner = calculateWinner(newCurrent.squares);

      this.updateDatabase(winner);
    });

    socket.on("resetGame", () => {
      this.resetGame();
    });
  }

  handleClick(i) {
    const history = this.state.history.slice(0, this.state.stepNumber + 1);
    const current = history[history.length - 1];
    const squares = current.squares.slice();

    if (
      calculateWinner(squares) ||
      squares[i] ||
      !(this.state.currentPlayer === this.state.player)
    ) {
      return;
    }
    this.props.socket.emit("makeMove", this.state.room, i, this.state.player);
  }

  resetGame() {
    let newPlayer = "O";
    if (newPlayer === this.state.player) {
      newPlayer = "X";
    }
    this.setState({
      stepNumber: 0,
      currentPlayer: "X",
      player: newPlayer,
      gameOver: false
    });
  }

  updateDatabase(winner) {
    if (!this.state.gameOver && winner) {
      this.props.addWinOrLossToUser(
        winner,
        this.props.username,
        this.state.player
      );
      this.setState({
        gameOver: true
      });
    }
  }

  newGame() {
    const history = this.state.history;
    const current = history[this.state.stepNumber];
    const winner = calculateWinner(current.squares);

    if (winner) {
      return (
        <div
          className="newGame"
          onClick={() => this.props.socket.emit("resetGame", this.state.room)}
        >
          <p className="divText">New Game</p>
        </div>
      );
    }
  }

  joinGame(room) {
    this.props.socket.emit("joinGame", room, false);
  }

  getRoomList() {
    let roomList = this.state.roomList;
    let html = [];

    for (let room in roomList) {
      if (roomList[room].length === 8)
        html.push(
          <button
            key={roomList[room]}
            onClick={() => this.joinGame(roomList[room])}
          >
            {roomList[room]}
          </button>
        );
    }
    return html;
  }

  render() {
    const history = this.state.history;
    const current = history[this.state.stepNumber];
    const winner = calculateWinner(current.squares);

    let status;
    if (winner) {
      status = "Winner: " + winner;
    } else {
      if (this.state.currentPlayer === this.state.player) status = "Your turn";
      else status = "Your opponents turn";
    }

    let gameBoardHtml = (
      <div className="game-board">
        <div>
          <div className="status">
            <p className="statusText">{status}</p>
          </div>
          <Board squares={current.images} onClick={i => this.handleClick(i)} />
        </div>

        <div className="game-info">
          {this.newGame()}
          <div
            className="leaveGame"
            onClick={() => this.props.socket.emit("leaveGame", this.state.room)}
          >
            <p className="divText">Leave</p>
          </div>
        </div>
      </div>
    );

    return <div className="game">{gameBoardHtml}</div>;
  }
}

function calculateWinner(squares) {
  let emptySquare = false;
  squares.forEach(square => {
    if (square === "" || square === null || square === "undefined") {
      emptySquare = true;
    }
  });

  if (!emptySquare) return "TIE";

  const lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6]
  ];
  for (let i = 0; i < lines.length; i++) {
    const [a, b, c] = lines[i];
    if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
      return squares[a];
    }
  }
  return null;
}

export default Game;
