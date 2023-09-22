#include "SgeStatus.h"
#include <QPoint>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

SgeStatus::SgeStatus()
{
    qDebug() << Q_FUNC_INFO;
}

int SgeStatus::getHeadX() const {
    return snake[0].x();
}

int SgeStatus::getHeadY() const {
    return snake[0].y();
}

int SgeStatus::getSnakeLength() const {
    return snake.length();
}

int SgeStatus::getAppleX() const {
    return apple.x();
}

int SgeStatus::getAppleY() const {
    return apple.y();
}

//retreive the status info when activeStatus.toJsonObject is called
QJsonObject SgeStatus::toJsonObject() const{
    QJsonObject json;
    QJsonArray snakeArr;

    qDebug() << "length of snake: " << snake.length();
    qDebug() << "snake jsonobj: " << snake;
    for (int i = 0; i < snake.length(); i++) {
        QPoint point = snake[i];
        qDebug() << "points in snake: " << point;
        QString snakeVal = "[" + QString::number(point.x()) + "," + QString::number(point.y()) + "]";
        snakeArr.append(snakeVal);
    }
    qDebug() << "snake arr sending to front end: " << snakeArr;

    json["state"] = state;
    json["direction"] = dir;
    json["apple"] = QString::number(apple.x()) + "," + QString::number(apple.y());
    json["snake"] = snakeArr;
    json["score"] = score;
    json["dateTime"] = dateTime.toString();

    return json;
}
