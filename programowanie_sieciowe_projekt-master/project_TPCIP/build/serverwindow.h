
#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QLineEdit>
#include <QBuffer>
#include <QFileDialog>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include "clickablelabel.h"

class ServerWindow : public QWidget {
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    QList<QTcpSocket*> connectedSockets;//list of connected clients

private slots:
    void onStartButtonClicked();//Start button click
    void newConnection();//Handling new connections
    void readMessage();//Read messages from the client
    void sendMessageToClient();//Send message to client
    void clientDisconnected();//Clear after disconected client
    void sendImageToClient();//Send image to client
    void clearChat();//Clear chat
    void shareScreen();//Share screen of desktop
    void showFullScreenImage();//Show bigger image
    void showFullScreenShare();//Show bigger image of screenshot
    void removeClient();//Remove currently connected client
    void stopServer();//Turn off server
    void toggleScreenshotSending();//On/off screenshot sending method

private:
    void paintEvent(QPaintEvent *event);//background setter
    QTcpSocket *socket;//Socket to connect to the server
    QTcpServer *tcpServer;//Pointer to the TCP server instance
    QLabel *statusLabel;//Label to display server status
    QTextEdit *messageLog;//Text area to display received messages
    QPushButton *startButton;//Button to start the server
    QLineEdit *messageInput;//Input for messages to client
    QPushButton *sendButton;//Button to send message to client
    QPushButton *sendImageButton;//Button to send image from server to client
    QPushButton *deleteButton;//Clear chat button
    QPixmap background;//background
    ClickableLabel *screenshotLabel;//Screenshot miniature
    ClickableLabel *imageLabel;//Image miniature
    QPushButton *sendScreenButton;//Desktop sharing
    QImage receivedScreenshot;//Ss in full quality
    QImage receivedImage;//Image in full quality
    QLabel *fullScreenImageLabel = nullptr;//full size image
    QLabel *fullScreenScreenshotLabel = nullptr;//full size desktop picture
    QTimer *screenshotTimer;//Screen is being send after this time
    bool sendingScreenshots = false;//Menage if you want to turn on/off screenshot sending
};
#endif // SERVERWINDOW_H
