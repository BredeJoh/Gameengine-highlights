import React from "react";
import dbManager from "./database";

let CLIENTURL = "http://localhost:3000";

if (process.env.NODE_ENV === "production") {
  CLIENTURL = "removed";
}

function LeaderboardEntry(props) {
  return (
    <div className={"leaderboardEntry"} id={props.user}>
      <p>
        {props.user} <br />
        {"Wins: " + props.wins + " - - - Losses: " + props.losses}
      </p>
    </div>
  );
}

class Leaderboard extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      entries: Array(10).fill(null)
    };
  }

  componentDidMount() {
    this.getTop10Entries();
  }

  async getTop10Entries() {
    let entries = await dbManager.getTop10ByWins();
    this.setState({
      entries: entries
    });
  }

  renderEntry(i) {
    if (this.state.entries === "undefined" || this.state.entries[i] === null)
      return;
    let entry = this.state.entries[i];
    if (entry !== null) {
      return (
        <LeaderboardEntry
          user={entry.RowKey._}
          wins={entry.wins._}
          losses={entry.losses._}
          ties={entry.ties._}
        />
      );
    }
  }

  render() {
    return (
      <div className="leaderboard">
        <div className="leaderboardText">Leaderboard</div>
        {this.renderEntry(0)}
        {this.renderEntry(1)}
        {this.renderEntry(2)}
        {this.renderEntry(3)}
        {this.renderEntry(4)}
        {this.renderEntry(5)}
        {this.renderEntry(6)}
        {this.renderEntry(7)}
        {this.renderEntry(8)}
        {this.renderEntry(9)}
      </div>
    );
  }
}

class Lobby extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      roomList: [],
      renderMode: 1,
      username: ""
    };
    this.setupEndPoints(this.props.socket);
    setInterval(() => this.props.socket.emit("getRoomList"), 2500);
  }

  updateUsername(name) {
    name.target.value = name.target.value.replace(/\s/g, "-");
    this.setState({
      username: name.target.value
    });
  }
  render() {
    let html;
    let usernameInput = (
      <input
        onChange={this.updateUsername.bind(this)}
        className="usernameInput"
        type="text"
        name="firstname"
        placeholder="Choose a username"
      />
    );
    switch (this.state.renderMode) {
      case 1:
        html = (
          <div className="lobby">
            <div>{usernameInput}</div>
            <div className="createGame" onClick={() => this.createGameRoom()}>
              <p className="divText">Create Game</p>
            </div>
            <div className="joinGame" onClick={() => this.changeRenderMode(3)}>
              <p className="divText">Find existing Games</p>
            </div>
          </div>
        );
        break;
      case 2:
        html = (
          <div className="lobby">
            <div>{usernameInput}</div>
            <div className="waiting">Waiting for player...</div>
            <div>
              <p>Invite link:</p>
            </div>
            <input
              type="url"
              readOnly={true}
              value={CLIENTURL + "?room=" + this.state.room}
            />
            <div
              className="goBack"
              onClick={() => {
                this.props.socket.emit("leaveGame", this.state.room);
              }}
            >
              <p className="divText">Go Back</p>
            </div>
          </div>
        );
        break;

      case 3:
        html = (
          <div className="lobby">
            <div>{usernameInput}</div>
            <div className="opponents">
              <p className="divText">Join opponents</p>
            </div>
            {this.getRoomList()}
            {/* <div><button onClick={() => this.props.socket.emit('getRoomList')}>refresh</button></div> */}
            <div className="goBack" onClick={() => this.changeRenderMode(1)}>
              <p className="divText">Go Back</p>
            </div>
          </div>
        );
        break;

      default:
        break;
    }

    return (
      <div>
        <div className="lobbyLeaderboard">
          <div className="grid1">{html}</div>
          <div className="grid2">{<Leaderboard />}</div>
        </div>
      </div>
    );
  }

  createGameRoom() {
    if (this.state.username !== "" && this.state.username !== "undefined")
      this.props.socket.emit("createGame", this.state.username);
    else window.alert("please enter a name before creating a game");
  }

  startGame() {
    this.props.renderMode();
    if (this.state.username !== "" || this.state.username !== "undefined")
      this.props.updateUsername(this.state.username);
  }

  getRoomList() {
    let roomList = this.state.roomList;
    let html = [];
    let i = 0;
    for (let room in roomList) {
      if (roomList[room].length !== 0 && roomList[room].length !== 20)
        html.push(
          <div key={i++}>
            <div
              className="roomEntry"
              key={roomList[room]}
              onClick={() => this.joinGame(roomList[room])}
            >
              {roomList[room]}
            </div>
          </div>
        );
    }
    return html;
  }

  changeRenderMode(mode) {
    this.setState({
      renderMode: mode
    });
  }

  joinGame(room) {
    if (this.state.username !== "" && this.state.username !== "undefined") {
      this.setState({
        room: room
      });
      this.props.socket.emit("joinGame", room);
    } else window.alert("please enter a name before joining a game");
  }

  setupEndPoints(socket) {
    socket.on("createGame", room => {
      this.setState({
        room: room
      });
      this.props.activeRoom(room);
      this.changeRenderMode(2);
    });

    socket.on("getRoomList", roomList => {
      this.setState({
        roomList: roomList
      });
    });

    socket.on("joinGame", room => {
      this.setState({
        room: room
      });
      this.props.activeRoom(room);
      this.props.socket.emit("startGame", this.state.room);
    });

    socket.on("leaveGame", room => {
      this.setState({
        room: "",
        renderMode: 1
      });
      this.props.activeRoom(room);
    });

    socket.on("startGame", () => {
      this.startGame();
    });
  }
}

export default Lobby;
