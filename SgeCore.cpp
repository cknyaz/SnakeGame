#include "SgeCore.h"
#include "SgeStatus.h"
#include "qjsonarray.h"
#include <QTcpSocket>
#include <QFile>
#include <QVector>
#include <QJsonDocument>
#include <iostream>
#include <cstdlib>

SgeCore::SgeCore(QObject *parent) : QObject(parent)
{

}

void SgeCore::start()
{
    // Init the status
    activeStatus.dateTime = QDateTime::currentDateTime();
    lastStatus = activeStatus;
    activeStatus.score = 0;
    activeStatus.state = GAME_OVER;

    // Start server
    connect(&server, &QTcpServer::newConnection, this, &SgeCore::newTcpClientConnected);
    server.listen(QHostAddress::Any, 8080);

    // Start tick generator
    connect(&tickGenerator, &QTimer::timeout, this, &SgeCore::tickStateMachine);
    tickGenerator.setInterval(300);
    tickGenerator.setTimerType(Qt::PreciseTimer);
    tickGenerator.setSingleShot(false);
    tickGenerator.start();
}


void SgeCore::newTcpClientConnected()
{
    qDebug() << Q_FUNC_INFO;

    // Get socket and configure
    QTcpSocket *s = server.nextPendingConnection();
    connect(s, &QTcpSocket::readyRead, this, &SgeCore::readClientBuffer);
    connect(s, &QTcpSocket::disconnected, this, &SgeCore::clientDisconnected);
    clients.append(s);
}

void SgeCore::readClientBuffer()
{
    // Get calling socket and gaurd against nullptrs
    QTcpSocket *s = getCallingSocket();
    if (s == nullptr)
    {
        return;
    }
    //read json message
    QByteArray jsonPacket;
    QByteArray buffer;
    QJsonObject jsonObj;

    // Append new data
    buffer += s->readAll();
    // While there is a null in the packet, parse the packets
    while (buffer.contains('\0'))
    {
        // Parse everything up until null, but keep everything after that
        jsonPacket = buffer.left( buffer.indexOf('\0') ); // Read everything before the null
        buffer = buffer.mid(jsonPacket.length() + 1); // Remove the captured + the null

        // Convert to a Json Obj
        jsonObj = QJsonDocument::fromJson(jsonPacket).object();
        if (jsonObj.isEmpty() == false) // Only parse if packet is not empty (Valid JSON)
        {
            QString command = jsonObj["command"].toString();
            QString direction = jsonObj["direction"].toString();

            //if the user requests the status, send the last status back by calling sendData()
            if (command == "getStatus") {
                sendData();
            }

            else {
                //if the user sent the message to restart a game; ie. stop the game, start a new game then go to the newGame func
                if (command == "restart") // Only parse if packet is not empty (Valid JSON)
                {
                    activeStatus.state = NEW_GAME;
                    newGame();
                }
                //if the user sent a message to the pause the game, we go to the pause game function
                else if (command == "pause") {
                    activeStatus.state = PAUSED;
                }
                //else the user sent a keypress, so change the direction based on that
    //            else if (command == "direction" && activeStatus.state != RUNNING) {
    //                newGame();
    //            }
                else {
                    //check isValidDirection here
                    if (command == "direction" && direction == "up") {
                        activeStatus.state = RUNNING;
                        activeStatus.dir = UP;
                        }
                    if (command == "direction" && direction == "down") {
                        activeStatus.state = RUNNING;
                            activeStatus.dir = DOWN;
                    }
                    if (command == "direction" && direction == "left") {
                            activeStatus.state = RUNNING;
                            activeStatus.dir = LEFT;
                    }
                    if (command == "direction" && direction == "right") {
                            activeStatus.state = RUNNING;
                            activeStatus.dir = RIGHT;
                    }
                }
                tickStateMachine();
            }
        }
    }
 }

void SgeCore::clientDisconnected()
{
    // Get calling socket and gaurd against nullptrs
    QTcpSocket *s = getCallingSocket()  ;
    if (s == nullptr)
    {
        return;
    }
    qDebug() << "Client Disconnected." << stdout;
 }


void SgeCore::tickStateMachine()
{
    activeStatus.dateTime = QDateTime::currentDateTime();

    //if the status is game over do nothing
    if (activeStatus.state == GAME_OVER)
    {
        // NOP
    }

    //if the user sent the message to restart a game; ie. stop the game, start a new game then go to the newGame func
    else if (activeStatus.state == NEW_GAME)
    {
//        newGame();
        //NOP
    }

    //else the user sent a keypress, so change the direction based on that
    else if (activeStatus.state == RUNNING)
    {
            moveSnake(activeStatus.dir);
    }

     lastStatus = activeStatus;
}

QTcpSocket *SgeCore::getCallingSocket()
{
    // Get sender and find the client that emitted the signal. If none is found, return nullptr
    QObject *sndr = sender();
    for (QTcpSocket *s : clients)
    {
        if (s == sndr)
        {
            return s;
        }
    }
    return nullptr;
}

bool SgeCore::isValidDirection(DIRECTION currentDirection, DIRECTION newDirection) {
    // Check if the new direction is not opposite or 180 degrees different
    if ((currentDirection == UP && newDirection == DOWN) ||
        (currentDirection == DOWN && newDirection == UP) ||
        (currentDirection == RIGHT && newDirection == LEFT) ||
        (currentDirection == LEFT && newDirection == RIGHT)) {
        return false;
    }
    return true;
}

void SgeCore::moveSnake(DIRECTION dir)
{
    bool collisionDetected = false;
    int dx = 0;
    int dy = 0;
    if (dir == LEFT) {
        dx = activeStatus.getHeadX() - 1;
        dy = activeStatus.getHeadY() + 0;
    }
    else if (dir == RIGHT) {
        dx = activeStatus.getHeadX() + 1;
        dy = activeStatus.getHeadY() + 0;
    }
    else if (dir == UP) {
        dx = activeStatus.getHeadX() + 0;
        dy = activeStatus.getHeadY() - 1;
    }
    else if (dir == DOWN) {
        dx = activeStatus.getHeadX() + 0;
        dy = activeStatus.getHeadY() + 1;
    }

    //check if apple was eaten
    if ((dx == activeStatus.getAppleX()) && (dy == activeStatus.getAppleY())) {
        collisionDetected = true;
        activeStatus.snake[0] = QPoint(activeStatus.getAppleX(), activeStatus.getAppleY());
        int len = activeStatus.getSnakeLength();
        int tailX = lastStatus.snake[len-1].x();
        int tailY = lastStatus.snake[len-1].y();

        activeStatus.snake.push_back(QPoint(tailX, tailY));
        //generate new apple
        generateApple();

        //gain 10 point
        activeStatus.score += 10;
    }

    //check if snake hit left wall
    else if (dx < 0) {
        gameOver();
    }

    //check if snake hit right wall
    else if (dx >= activeStatus.boardWidth) {
        gameOver();
    }

    //check if snake hit top wall
    else if (dy < 0) {
        gameOver();
    }
    //check if snake hit bottom wall
    else if (dy >= activeStatus.boardHeight) {
        gameOver();
    }
    // you win
    else if (activeStatus.getSnakeLength() >= (activeStatus.boardHeight * activeStatus.boardWidth)) {
        qDebug() << "you win!";
        activeStatus.state = WIN;

        //gain 10 points
        activeStatus.score += 10;
    }
    //check if snake has eaten itself
    else {
        // NOTE: starting at 3 bc it's impossible to eat its own head or the next 2 elements
        for (int i = 3; i < activeStatus.getSnakeLength() - 1; i++) {
            if ((activeStatus.snake[i].x() == dx) && (activeStatus.snake[i].y() == dy)) {
                gameOver();
            }
        }
    }

    if (activeStatus.state == RUNNING && collisionDetected == false) {
        //    set the new snake head
        activeStatus.snake.push_front(QPoint(dx, dy));
        activeStatus.snake.pop_back();
    }
}

void SgeCore::newGame() {
    //generate new snake with location (5, 5)
    QVector<QPoint> newSnake;
    QPoint singlePoint(5, 5);
    newSnake.append(singlePoint);
    activeStatus.snake = newSnake;

    //generate apple location
    generateApple();

    //set direction to right
    activeStatus.dir = RIGHT;

    //set game state to running
    activeStatus.state = RUNNING;
}

void SgeCore::pauseGame(bool isRunning) {
    activeStatus.state = PAUSED;
}

void SgeCore::gameOver() {
    //decrement score - 10
    //change state to GAME_OVER, end game
    qDebug() << "game over";
    activeStatus.state = GAME_OVER;

    //lose 10 points
    activeStatus.score -= 10;
}

void SgeCore::generateApple() {
    //add random apple within the bounds of the board
    int numAvailSpots = activeStatus.boardWidth * activeStatus.boardHeight - activeStatus.getSnakeLength();
    int i = 0;

    //TODO: seed rand
    int appleId = rand() % numAvailSpots;

    //check if the apple is not inside snake coordinates
    for (int y = 0; y < activeStatus.boardHeight; y++) {
        for (int x = 0; x < activeStatus.boardWidth; x++) {
            if (i == appleId) {
                activeStatus.apple = QPoint(x, y);
                return;
            }
            if (activeStatus.snake.contains(QPoint(x, y)) == false) {
                i++;
            }
        }
    }
}

void SgeCore::sendData() {
   //TODO: send status to frontend via json object and tcp port
    QJsonObject statusJson = lastStatus.toJsonObject();

    QByteArray jsonByteArray = QJsonDocument(statusJson).toJson(QJsonDocument::Compact);
    jsonByteArray.append('\0');

    QTcpSocket *s = getCallingSocket();
    s->write(jsonByteArray);

}
