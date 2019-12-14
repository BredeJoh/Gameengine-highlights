import React from "react";
import Game from "./Ticktacktoe";
import io from "socket.io-client";
import dbManager from "./database";
import Lobby from "./lobby";
import "./App.css";

let APIURL = "http://localhost:1337";

if (process.env.NODE_ENV === "production") {
  APIURL = "removed";
}

const renderModeEnum = {
  LOBBY: 1,
  GAME: 2
};

const socket = io(APIURL);

class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      renderMode: renderModeEnum.LOBBY,
      activeRoom: "",
      isHosting: false
    };
    this.updateUsername = this.updateUsername.bind(this);
    this.addWinOrLossToUser = this.addWinOrLossToUser.bind(this);

    socket.on("createGame", () => {
      this.setState({
        isHosting: true
      });
    });
  }

  componentDidMount() {
    const urlParams = new URLSearchParams(window.location.search);
    const roomName = urlParams.get("room");
    if (roomName) {
      let name = "guest";
      do {
        name = prompt(
          "Please enter a username",
          "guest" + Math.floor(Math.random() * 10000)
        );
      } while (name === "");
      this.updateUsername(name);
      socket.emit("joinGame", roomName);
    }
  }

  updateUsername(name) {
    if (name !== "" && name !== "undefined") {
      name = name.replace(/\s/g, "-");
      this.setState({
        username: name
      });
    }
  }

  addWinOrLossToUser(winner, username, playerRole) {
    if (winner) {
      if (winner === "TIE") dbManager.addStatsToUser(username, 0, 0, 1);
      else if (winner === playerRole)
        // WIN
        dbManager.addStatsToUser(username, 1, 0, 0);
      // LOSS
      else dbManager.addStatsToUser(username, 0, 1, 0);
    }
  }

  updateRoom(room) {
    this.setState({
      activeRoom: room
    });
  }

  getIsHosting() {
    return this.state.isHosting;
  }

  getRoom() {
    return this.state.activeRoom;
  }

  updateRenderMode() {
    this.setState({
      renderMode: 2
    });
  }
  render() {
    let htmlToRender;

    switch (this.state.renderMode) {
      case renderModeEnum.LOBBY:
        htmlToRender = (
          <Lobby
            socket={socket}
            renderMode={this.updateRenderMode.bind(this)}
            activeRoom={this.updateRoom.bind(this)}
            updateUsername={this.updateUsername}
          />
        );
        break;

      case renderModeEnum.GAME:
        htmlToRender = (
          <Game
            socket={socket}
            getRoom={this.getRoom.bind(this)}
            isHosting={this.getIsHosting.bind(this)}
            username={this.state.username}
            addWinOrLossToUser={this.addWinOrLossToUser}
          />
        );
        break;

      default:
        htmlToRender = (
          <Lobby
            socket={socket}
            renderMode={this.updateRenderMode.bind(this)}
            activeRoom={this.updateRoom.bind(this)}
            updateUsername={this.updateUsername}
          />
        );
        break;
    }

    return <div className="App">{htmlToRender}</div>;
  }
}

export default App;
