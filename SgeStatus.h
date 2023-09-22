#ifndef SGESTATUS_H
#define SGESTATUS_H

#include <QJsonObject>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QPoint>

enum GAME_STATE
{
    NEW_GAME, //0
    RUNNING, //1
    PAUSED,  //2
    GAME_OVER,  //3
    WIN //4
};

enum DIRECTION
{
    UP,  //0
    DOWN,  //1
    LEFT,  //2
    RIGHT  //3
};

// Struct
class SgeStatus
{
public:
    GAME_STATE state = NEW_GAME;
    QVector<QPoint> snake; //Default to 1
    QPoint apple;
    DIRECTION dir = RIGHT;
    QDateTime dateTime;
    int score;
    int boardWidth = 20;
    int boardHeight = 20;

public:
    // Constructor / desconstructor
    SgeStatus();
    SgeStatus(const QJsonObject &jsonObj);

    // Helper functions
    int getHeadX() const;
    int getHeadY() const;
    int getSnakeLength() const;
    int getAppleX() const;
    int getAppleY() const;



    // JSON
    QJsonObject toJsonObject() const;
};

#endif // SGESTATUS_H
