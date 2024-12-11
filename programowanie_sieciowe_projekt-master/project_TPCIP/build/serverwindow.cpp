#include "serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Server Window");

    const int x=860;//X window size
    const int y=860;//Y window size

    setFixedSize(x, y);//Set window size
    //Page background
    background = QPixmap("/home/krzysiek89/Desktop/QT_aplikacje/Programowanie_sieciowe/programowanie_sieciowe_projekt-master/project_TPCIP/bck.jpg").scaled(x, y);

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
    screenshotTimer->setInterval(100);

    //Desktop sharing
    QPushButton *sendScreenshotButton = new QPushButton("Send Screenshot", this);
    sendScreenshotButton->setGeometry(510, 760, 120, 50);
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ServerWindow::toggleScreenshotSending);

    screenshotLabel = new ClickableLabel(this);//Screenshot got
    connect(screenshotLabel, &ClickableLabel::clicked, this, &ServerWindow::showFullScreenShare);

    imageLabel = new ClickableLabel(this);//Image got
    connect(imageLabel, &ClickableLabel::clicked, this, &ServerWindow::showFullScreenImage);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ServerWindow::readMessage);
    connect(socket, &QTcpSocket::disconnected, this, &ServerWindow::clientDisconnected);

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
    // Check if the server is running
    if (tcpServer->isListening()) {
        // Stop the server from listening to new connections
        tcpServer->close();

        // Update the status label
        statusLabel->setText("Server stopped listening to new connections");
        messageLog->append("Server has stopped listening for new clients.");
    } else {
        messageLog->append("Server is already not listening.");
    }
}

// Read message from client
void ServerWindow::readMessage() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray data = socket->readAll();

    if (data.isEmpty()) {
        qDebug() << "No data received.";
        return;
    }

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

            //Update view on bigger screen to be the most actual one
            if (fullScreenScreenshotLabel && fullScreenScreenshotLabel->isVisible()) {
                QPixmap scaledPixmap = QPixmap::fromImage(receivedScreenshot.scaled(960, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                fullScreenScreenshotLabel->setPixmap(scaledPixmap);
                fullScreenScreenshotLabel->update();
                fullScreenScreenshotLabel->repaint();
            }
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
                connectedSockets.removeOne(socket);
                socket->deleteLater();
            }
        }
        messageLog->append("Sent message to client: " + message);
        messageInput->clear();
    }
}

void ServerWindow::clientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    connectedSockets.removeOne(socket);//Delete disconected client from active users list
    socket->disconnectFromHost();//Disconnect from host
    messageLog->append("Client disconnected.");
}

void ServerWindow::sendImageToClient() {
    //Dialog to choose image file
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (fileName.isEmpty()) {
        return;//Finish if not taken
    }
    QImage image(fileName);//Use choosen file
    if (image.isNull()) {
        qDebug() << "Invalid or empty image.";
        return;
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "JPEG", 60)) {
        qDebug() << "Failed to save image to buffer.";
        return;
    }

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
        if (screenshot.save(&buffer, "JPEG",60)) {//60 is compression quality (0-100max)
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

void ServerWindow::onFullScreenScreenshotLabelDestroyed() {
    fullScreenScreenshotLabel = nullptr;
}

void ServerWindow::onFullImageLabelDestroyed() {
    fullScreenImageLabel = nullptr;
}

void ServerWindow::showFullScreenImage() {//Full size of image got
    if (receivedImage.isNull()) {
        return;//Img exist?
    }

    if (fullScreenImageLabel) {//if photo exist, close old one
        fullScreenImageLabel->close();
        fullScreenImageLabel = nullptr;
    }

    //new qlabel to display
    fullScreenImageLabel = new QLabel;
    fullScreenImageLabel->setFixedSize(960, 960);//new window size

    //Scale screenshot
    QPixmap scaledPixmapImage = QPixmap::fromImage(receivedImage.scaled(960,960, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenImageLabel->setPixmap(scaledPixmapImage);

    fullScreenImageLabel->setWindowTitle("Image Preview");
    fullScreenImageLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenImageLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenImageLabel->show();

    //set nullptr after QLabel closing
    connect(fullScreenImageLabel, &QLabel::destroyed, this, &ServerWindow::onFullImageLabelDestroyed);
}

void ServerWindow::showFullScreenShare() {//Full size of screenshot got
    if (receivedScreenshot.isNull()) {
        return;//Img exist?
    }

    if (fullScreenScreenshotLabel) {//if photo exist, close old one
        fullScreenScreenshotLabel->close();
        fullScreenScreenshotLabel=nullptr;
    }

    //new qlabel to display
    fullScreenScreenshotLabel = new QLabel;
    fullScreenScreenshotLabel->setFixedSize(960, 960);//new window size

    //Scale screenshot
    QPixmap scaledPixmapScreenshot = QPixmap::fromImage(receivedScreenshot.scaled(960, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenScreenshotLabel->setPixmap(scaledPixmapScreenshot);

    fullScreenScreenshotLabel->setWindowTitle("Screenshot Preview");
    fullScreenScreenshotLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenScreenshotLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenScreenshotLabel->show();

    //set nullptr after QLabel closing
    connect(fullScreenScreenshotLabel, &QLabel::destroyed, this, &ServerWindow::onFullScreenScreenshotLabelDestroyed);
}

void ServerWindow::toggleScreenshotSending() {
    sendingScreenshots = !sendingScreenshots;//Change flag(on/off screenshot sending)

    if (sendingScreenshots) {
        screenshotTimer->start();//Turn on timer
    } else {
        screenshotTimer->stop();//Stop timer
    }
}
