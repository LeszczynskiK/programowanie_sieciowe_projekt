#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class ServerWindow : public QWidget {
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);

private slots:
    void onStartButtonClicked();//Start button click
    void newConnection();//Handling new connections
    void readMessage();//Read messages from the client

private:
    QTcpServer *tcpServer;//Pointer to the TCP server instance
    QLabel *statusLabel;//Label to display server status
    QTextEdit *messageLog;//Text area to display received messages
    QPushButton *startButton;//Button to start the server
};
#endif // SERVERWINDOW_H
