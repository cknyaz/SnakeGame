#ifndef SGECORE_H
#define SGECORE_H

#include <QObject>
#include <QTimer>
#include <QTcpServer>

#include "SgeStatus.h"

class SgeCore : public QObject
{
    Q_OBJECT
public:
    // Constructor / deconstructor
    explicit SgeCore(QObject *parent = nullptr);

    // API
    void start();

public slots:

signals:

private slots:
    void newTcpClientConnected();       // Called by server. New TCP connection is made
    void readClientBuffer();            // Called by clients. Client sent a JSON message to core
    void clientDisconnected();          // Called by clients when they disconnect from core
    void tickStateMachine();            // Called by QTimer to tick the engine every few ms

private:
    // Functions called by client if they send a valid JSON message
    void newGame();
    void pauseGame(bool paused);

    // Helper functions for the state machine
    bool isValidDirection(DIRECTION currentDirection, DIRECTION newDirection);
    void moveSnake(DIRECTION dir);
    void generateApple();
    void gameOver();

    // Other helper functions
    QTcpSocket *getCallingSocket();
    void sendData();

private:
    SgeStatus activeStatus;             // Active status, known only to core
    SgeStatus lastStatus;               // When a client wants the status, they get this
    QTimer tickGenerator;
    QTcpServer server;
    QVector<QTcpSocket *> clients;
};

#endif // SGECORE_H
