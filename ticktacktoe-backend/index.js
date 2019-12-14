var app = require("express")();
var port = process.env.PORT || 1337;
var http = require("http").Server(app);
var io = require("socket.io")(http);
app.get("/", function (req, res) {
    res.sendfile("index.html");
});
io.origins(["*:*"]);
io.on("connection", function (socket) {
    socket.on("createGame", function (room) {
        if (room === "" ||
            room === "undefined" ||
            roomHelper.checkIfRoomExists(room))
            room = roomHelper.generateRoomName();
        socket.join(room);
        io.sockets.to(socket.id).emit("createGame", room);
    });
    socket.on("joinRoom", function (room) {
        socket.join(room);
        io.sockets.to(socket.id).emit("joinRoom", room);
    });
    socket.on("joinGame", function (room) {
        if (room === "") {
            room = roomHelper.generateRoomName();
            socket.join(room);
            io.sockets.to(socket.id).emit("joinGame", room);
        }
        else if (roomHelper.checkIfRoomExists(room) &&
            roomHelper.getNumOfConnectionsInRoom(room) < 2) {
            socket.join(room);
            io.sockets.to(socket.id).emit("joinGame", room);
        }
        else {
            console.log("failed to join room: " + room);
        }
    });
    socket.on("startGame", function (room) {
        io.sockets.in(room).emit("startGame");
    });
    socket.on("leaveGame", function (room) {
        io.sockets.in(room).emit("leaveGame");
        socket.leave(room);
    });
    socket.on("makeMove", function (room, index, player) {
        io.sockets.in(room).emit("makeMove", index, player);
    });
    socket.on("resetGame", function (room) {
        io.sockets.in(room).emit("resetGame");
    });
    socket.on("getRoomList", function () {
        io.sockets.to(socket.id).emit("getRoomList", roomHelper.findRooms());
    });
});

http.listen(port, function () {
    console.log("listening on *:" + port);
});
let roomHelper = {
    findRooms: (roomLimit = 2) => {
        var availableRooms = [];
        var rooms = io.sockets.adapter.rooms;
        if (rooms) {
            for (var room in rooms) {
                if (!rooms[room].hasOwnProperty(room) &&
                    roomHelper.getNumOfConnectionsInRoom(room) < roomLimit) {
                    availableRooms.push(room);
                }
            }
        }
        return availableRooms;
    },
    checkIfRoomExists(room) {
        let availableRooms = this.findRooms();
        for (var _room in availableRooms) {
            if (room === availableRooms[_room])
                return true;
        }
        return false;
    },
    getNumOfConnectionsInRoom: _room => {
        if (io.sockets.adapter.rooms[_room])
            return io.sockets.adapter.rooms[_room].length;
        else
            return 0;
    },
    generateRoomName: () => {
        var roomName = "";
        var charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        for (var i = 0; i < 8; i++)
            roomName += charSet.charAt(Math.floor(Math.random() * charSet.length));
        return roomName;
    }
};
