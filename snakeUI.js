var interval;
var blockSize = 25;
var total_row = 20; //total row number
var total_col = 20; //total column number
var board;
var context;

var snakeX = blockSize * 5;
var snakeY = blockSize * 5;

// Set the total number of rows and columns
var speedX = 0; //speed of snake in x coordinate.
var speedY = 0; //speed of snake in Y coordinate.

var snakeBody = [];

var foodX;
var foodY;
var gameOver = false;

window.onload = function () {
  document.addEventListener("keyup", changeDirection); //for movements
  document.getElementById("newGame").addEventListener("click", () => {
    newGame("restart");
  });
};

function parseData(input) {
  var json = {};

  var appleArr = input.replace(/"/g, "");
  appleArr = appleArr.split("apple:")[1].split(",dateTime")[0];
  appleArr = JSON.parse("[" + appleArr + "]");

  var snake = input.replace(/"/g, "");
  snake = snake.split("snake:")[1].split(",state")[0].split(",");
  snake = JSON.parse(snake);

  json.apple = appleArr;
  json.dateTime = input.split('"dateTime":')[1].split(',"direction"')[0];
  json.direction = input.split('"direction":')[1].split(',"score"')[0];
  json.score = input.split('"score":')[1].split(',"snake"')[0];
  json.snake = snake;
  json.state = input.split('"state":')[1].split('"}"')[0][0];

  return json;
}

function update(data) {
  if (data.state == 3) {
    clearInterval(interval);
    alert("Game Over");
    return;
  }
  // Set board height and width
  board = document.getElementById("board");
  board.height = total_row * blockSize;
  board.width = total_col * blockSize;
  context = board.getContext("2d");

  // Background of a Game
  context.fillStyle = "green";
  context.fillRect(0, 0, board.width, board.height);

  // Set food color and position
  context.fillStyle = "yellow";
  foodX = data.apple[0] * blockSize;
  foodY = data.apple[1] * blockSize;

  context.fillRect(foodX, foodY, blockSize, blockSize);

  // Set snake color and position
  context.fillStyle = "white";
  snakeBody = Array.from(data.snake);

  snakeX = snakeBody[0][0] * blockSize; //updating Snake position in X coordinate.
  snakeY = snakeBody[0][1] * blockSize; //updating Snake position in Y coordinate.

  context.fillRect(snakeX, snakeY, blockSize, blockSize);

  console.log("snakeBody[0][0] " + snakeBody[0][0]);
  console.log("snakeBody[0][1] " + snakeBody[0][1]);

  snakeBody.forEach(function (cell, index) {
    context.fillRect(
      cell[0] * blockSize,
      cell[1] * blockSize,
      blockSize,
      blockSize
    );
  });
}

// Movement of the Snake - We are using addEventListener
function changeDirection(e) {
  if (e.code == "ArrowUp") {
    sendDirection("up");
  } else if (e.code == "ArrowDown") {
    sendDirection("down");
  } else if (e.code == "ArrowLeft") {
    sendDirection("left");
  } else if (e.code == "ArrowRight") {
    sendDirection("right");
  }
}

function sendDirection(dir) {
  const url = "http://localhost:5500/api/changeDirection?direction=" + dir + "";
  fetch(url)
    .then((response) => response.text())
    .then((json) => json);
}

function newGame(status) {
  gameOver = false;
  interval = setInterval(() => {
    getStatus();
  }, 300);

  const url = "http://localhost:5500/api/newGame";
  fetch(url)
    .then((response) => response.text())
    .then((json) => json);
}

function getStatus() {
  fetch("http://localhost:5500/api/getStatus")
    .then((response) => response.text())
    .then((data) => {
      if (data.length > 0) {
        // console.log("data: " + data);
        var parsed = parseData(data);
        update(parsed);
      }
    })
    .catch((error) => {
      console.log("Fetch error: " + error);
    });
}
