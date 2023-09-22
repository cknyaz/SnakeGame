const net = require("net");
const express = require("express");
const path = require("path");
const app = express();
var cors = require("cors");
const PORT = 5500;

app.use(cors());
var client = new net.Socket();

client.connect(8080, "127.0.0.1", function () {
  console.log("Connected");
  client.write("Hello from client.");
});

client.on("close", function () {
  console.log("Connection closed");
});

client.on("data", (data) => {
  // Buffer the data received from the TCP connection
  bufferData(data);
});

app.get("/", (req, res) => {
  res.sendStatus(0);
  console.log("status sent from nodejs");
});

app.listen(PORT, function (err) {
  if (err) console.log(err);
  console.log("Server is listening on PORT", PORT);
});

app.get("/api/newGame", (req, res) => {
  let dateTime = new Date();
  let datestr = dateTime.toISOString();

  const jsonData = {
    command: "restart",
    status: "restart",
    direction: "",
    dateTime: datestr,
  };

  const jsonStr = JSON.stringify(jsonData) + "\0";
  console.log("sending restart command from nodejs to c++ server: " + jsonStr);
  client.write(jsonStr);
  res.send("send request to core to start a new game");
});

app.get("/api/changeDirection", (req, res) => {
  const direction = req.query.direction;
  let dateTime = new Date();
  let datestr = dateTime.toISOString();

  const jsonData = {
    command: "direction",
    status: "running",
    direction: direction,
    dateTime: datestr,
  };
  console.log("direction from nodejs when changing direction: " + direction);
  const jsonStr = JSON.stringify(jsonData) + "\0";
  console.log("sending direction from nodejs to c++ server: " + jsonStr);
  client.write(jsonStr);
  res.send("send request to core to change direction of snake");
});

app.get("/api/getStatus", (req, res) => {
  let dateTime = new Date();
  let datestr = dateTime.toISOString();

  const jsonData = {
    command: "getStatus",
    status: "getStatus",
    direction: "",
    dateTime: datestr,
  };
  const jsonStr = JSON.stringify(jsonData) + "\0";
  client.write(jsonStr);

  const interval = setInterval(() => {
    sendBufferedDataToClient(res);
  }, 300);

  res.on("close", () => {
    clearInterval(interval);
  });
});

let dataBuffer = "";

function bufferData(data) {
  dataBuffer += data.toString();
  console.log("received data from core: " + dataBuffer);
}
function sendBufferedDataToClient(clientSocket) {
  if (dataBuffer.length > 0) {
    console.log("sending buffer data to front end: " + dataBuffer);
    clientSocket.send(dataBuffer);
    dataBuffer = ""; // Clear the buffer after sending data
  }
}
