#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QDebug>

    class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);
    void connectToServer(const QString &ipAddress);
    void sendMessage(const QString &message);

private slots:
    void readMessage();

private:
    QTcpSocket *socket;
};

#endif // CLIENT_H
