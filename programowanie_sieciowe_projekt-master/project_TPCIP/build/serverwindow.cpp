#include "serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Server Window");

    const int x=860;//X window size
    const int y=860;//Y window size

    setFixedSize(x, y);//Set window size
    //Page background
    background = QPixmap("/home/krzysiek89/Desktop/QT_aplikacje/Programowanie_sieciowe/programowanie_sieciowe_projekt-master/project_TPCIP/bck.jpg").scaled(x, y);

    QList<QTcpSocket*> connectedSockets;//list of connected clients

    QFont font;
    font.setPointSize(15);//Font size - beginning initialization

    //Server status label
    statusLabel = new QLabel("Server is not running", this);
    statusLabel->setStyleSheet("color: yellow;");//Colour of the text
    statusLabel->setGeometry(10, 10, 600, 200);
    statusLabel->setFont(font);

    //Message log field
    messageLog = new QTextEdit(this);
    messageLog->setReadOnly(true);
    messageLog->setGeometry(10, 120, 540, 540);
    messageLog->setFont(font);

    //Start server button
    startButton = new QPushButton("Start Server", this);
    startButton->setGeometry(10, 760, 150, 50);
    startButton->setFont(font);
    connect(startButton, &QPushButton::clicked, this, &ServerWindow::onStartButtonClicked);

    tcpServer = new QTcpServer(this);//Initialize TCP server

    messageInput = new QLineEdit(this);//Server can type to client
    messageInput->setGeometry(10, 680, 400, 50);
    messageInput->setFont(font);

    sendButton = new QPushButton("Send to Client", this);//Send do client button
    sendButton->setGeometry(420, 680, 150, 50);
    sendButton->setFont(font);
    connect(sendButton, &QPushButton::clicked, this, &ServerWindow::sendMessageToClient);

    sendImageButton = new QPushButton("Send Image to Client", this);
    sendImageButton->setGeometry(180, 760, 150, 50);;
    connect(sendImageButton, &QPushButton::clicked, this, &ServerWindow::sendImageToClient);

    // Initialize delete button
    QPushButton *deleteButton = new QPushButton("Clear Chat", this);
    deleteButton->setFont(font);
    deleteButton->setGeometry(360, 760, 120, 50); // Geometry for the delete button
    connect(deleteButton, &QPushButton::clicked, this, &ServerWindow::clearChat);


    //Remove currently connected client button
    QPushButton *removeClientButton = new QPushButton("Remove connections", this);
    removeClientButton->setFont(font);
    removeClientButton->setGeometry(340, 50, 220, 50);
    connect(removeClientButton, &QPushButton::clicked, this, &ServerWindow::removeClient);

    //Stop server button
    QPushButton *stopServerButton = new QPushButton("Stop Server", this);
    stopServerButton->setFont(font);
    stopServerButton->setGeometry(600, 50, 120, 50);
    connect(stopServerButton, &QPushButton::clicked, this, &ServerWindow::stopServer);


    //Add frame based on QLabel
    QLabel *frame = new QLabel(this);
    frame->setGeometry(600, 210, 180, 180);//Pos and size
    frame->setStyleSheet("border: 2px solid grey;");//Style of frame

    QLabel *frame1 = new QLabel(this);
    frame1->setGeometry(600, 450, 180, 180);//Pos and size
    frame1->setStyleSheet("border: 2px solid grey;");//Style of frame

    screenshotTimer = new QTimer(this);
    connect(screenshotTimer,&QTimer::timeout, this, &ServerWindow::shareScreen);
    screenshotTimer->setInterval(1000);

    //Desktop sharing
    QPushButton *sendScreenshotButton = new QPushButton("Send Screenshot", this);
    sendScreenshotButton->setGeometry(510, 760, 120, 50);
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ServerWindow::toggleScreenshotSending);

    screenshotLabel = new ClickableLabel(this);//Screenshot got
    connect(screenshotLabel, &ClickableLabel::clicked, this, &ServerWindow::showFullScreenShare);

    imageLabel = new ClickableLabel(this);//Image got
    connect(imageLabel, &ClickableLabel::clicked, this, &ServerWindow::showFullScreenImage);

}

void ServerWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);//background paint
    QWidget::paintEvent(event);
}

//Start button click
void ServerWindow::onStartButtonClicked() {
    if (tcpServer->listen(QHostAddress::Any, 12353)) {
        statusLabel->setText("Server is listening...");
        connect(tcpServer, &QTcpServer::newConnection, this, &ServerWindow::newConnection);
    } else {
        statusLabel->setText("Server could not start! Error: " + tcpServer->errorString());
    }
}

//Handle new connections
void ServerWindow::newConnection() {
    QTcpSocket *socket = tcpServer->nextPendingConnection();//Get connection with new client
    connectedSockets.append(socket);//Add client to connected clients list
    connect(socket, &QTcpSocket::readyRead, this, &ServerWindow::readMessage);//Hear message
    connect(socket, &QTcpSocket::disconnected, this, &ServerWindow::clientDisconnected);//Delete cliend after disconecting(client)
    messageLog->append("Client connected.");
}

// Remove currently connected clients
void ServerWindow::removeClient() {
    if (!connectedSockets.isEmpty()) {
        for (QTcpSocket *socket : connectedSockets) {
            if (socket->state() == QAbstractSocket::ConnectedState) {
                socket->write("forced_stop");//Send information to client
                socket->disconnectFromHost();//Disconnect client
                socket->waitForDisconnected();//Wait for disconnection
                socket->deleteLater();//Delete socket
            }
        }
        connectedSockets.clear();//Clear connected socked list
        messageLog->append("Removed clients");
    } else {
        messageLog->append("No clients to remove");
    }
}

//Stop the server
void ServerWindow::stopServer() {
    //Close sockets for clients
    for (QTcpSocket *clientSocket : connectedSockets) {
        if (clientSocket) {
            clientSocket->write("forced_stop");//Send information to client
            clientSocket->disconnectFromHost();//End connection
            clientSocket->waitForDisconnected();//Wait for full disconnection
            clientSocket->deleteLater();//Mark socket for deletion
        }
    }

    //Close the server
    if (tcpServer) {
        tcpServer->close();//Stop listening
    }

    connectedSockets.clear();//Clear the connection list

    //Update the status label
    statusLabel->setText("Server is not running");
    messageLog->append("Server stopped");
}


// Read message from client
void ServerWindow::readMessage() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray data = socket->readAll();
    QImage image;

    //Img exist?
    if (image.loadFromData(data)) {
        //set specificity
        imageLabel->setPixmap(QPixmap::fromImage(image.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        imageLabel->setGeometry(608, 218, 160, 160);
        imageLabel->show();

        //save gotten img
        receivedImage = image;//keep to later use
    } else if (data.startsWith("SCREENSHOT")) { //is it ss
        QByteArray screenshotData = data.mid(10);
        QImage screenshotImage;
        if (screenshotImage.loadFromData(screenshotData)) {
            //actualise pixmap
            screenshotLabel->setPixmap(QPixmap::fromImage(screenshotImage.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            screenshotLabel->setGeometry(608, 450, 160, 160);
            screenshotLabel->show();

            //save img
            receivedScreenshot = screenshotImage; //keep to later use
        }
    } else {
        QString serverMessage = QString::fromUtf8(data);
        messageLog->setTextColor(Qt::white);
        messageLog->append("Server: " + serverMessage);
    }
}



void ServerWindow::sendMessageToClient() {//Send message
    QString message = messageInput->text();
    if (!message.isEmpty()) {//
        for (QTcpSocket *socket : connectedSockets) {//Look for connected socket
            if (socket->state() == QAbstractSocket::ConnectedState) {
                socket->write(message.toUtf8());//write
                socket->flush();//Flush socket
            }
            else
            {
                qDebug("Socket not connected!");
            }
        }
        messageLog->append("Sent message to client: " + message);
        messageInput->clear();
    }
}

void ServerWindow::clientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    connectedSockets.removeOne(socket);//Delete disconected client from active users list
    socket->deleteLater();//Delete socket
    messageLog->append("Client disconnected.");
}

void ServerWindow::sendImageToClient() {
    //Dialog to choose image file
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (fileName.isEmpty()) {
        return;//Finish if not taken
    }
    QImage image(fileName);//Use choosen file
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG");//Save as jpeg

    //Send image to all connected sockets
    for (QTcpSocket *socket : connectedSockets) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(byteArray);//Send
            socket->flush();//Push
        }
    }
}

void ServerWindow::clearChat() {//Clear messages
    messageLog->clear();
}

void ServerWindow::shareScreen() {//Send screenshot of desktop
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QPixmap screenshot = screen->grabWindow(0);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (buffer.open(QIODevice::WriteOnly)) {
        if (screenshot.save(&buffer, "JPEG")) {
            //Send to connected sockets
            for (QTcpSocket *socket : connectedSockets) {
                if (socket->state() == QAbstractSocket::ConnectedState) {
                    QByteArray dataToSend = "SCREENSHOT" + byteArray;
                    socket->write(dataToSend);
                    socket->flush();
                } else {
                    messageLog->append("Socket not connected.");
                }
            }
        } else {
            messageLog->append("Failed to save screenshot.");
        }
    } else {
        messageLog->append("Failed to open buffer.");
    }
}

void ServerWindow::showFullScreenImage() {//Full size of image got
    if (receivedScreenshot.isNull()) {
        return;//Img exist?
    }

    //new qlabel to display
    QLabel *fullScreenLabel = new QLabel;
    fullScreenLabel->setFixedSize(640, 640);//new window size

    //Scale screenshot
    QPixmap scaledPixmap = QPixmap::fromImage(receivedImage.scaled(640, 640, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenLabel->setPixmap(scaledPixmap);

    fullScreenLabel->setWindowTitle("Image Preview");
    fullScreenLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenLabel->show();
}

void ServerWindow::showFullScreenShare() {//Full size of screenshot got
    if (receivedScreenshot.isNull()) {
        return;//Img exist?
    }

    //new qlabel to display
    QLabel *fullScreenLabel = new QLabel;
    fullScreenLabel->setFixedSize(640, 640);//new window size

    //Scale screenshot
    QPixmap scaledPixmap = QPixmap::fromImage(receivedScreenshot.scaled(640, 640, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenLabel->setPixmap(scaledPixmap);

    fullScreenLabel->setWindowTitle("Screenshot Preview");
    fullScreenLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenLabel->show();
}

void ServerWindow::toggleScreenshotSending() {
    sendingScreenshots = !sendingScreenshots;//Change flag(on/off screenshot sending)

    if (sendingScreenshots) {
        screenshotTimer->start();//Turn on timer
    } else {
        screenshotTimer->stop();//Stop timer
    }
}
