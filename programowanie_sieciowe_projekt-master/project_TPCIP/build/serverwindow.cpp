#include "serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Server Window");

    const int x =860;
    const int y=860;

    setFixedSize(x, y);//Window size
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

    // Connect the delete button signal to the corresponding slot
    connect(deleteButton, &QPushButton::clicked, this, &ServerWindow::clearChat);

    //Add frame based on QLabel
    QLabel *frame = new QLabel(this);
    frame->setGeometry(600, 210, 180, 180);//Pos and size
    frame->setStyleSheet("border: 2px solid grey;");//Style of frame

    QLabel *frame1 = new QLabel(this);
    frame1->setGeometry(600, 450, 180, 180);//Pos and size
    frame1->setStyleSheet("border: 2px solid grey;");//Style of frame

    //Desktop sharing
    QPushButton *sendScreenshotButton = new QPushButton("Send Screenshot", this);
    sendScreenshotButton->setGeometry(510, 760, 120, 50);
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ServerWindow::shareScreen);
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
// Read message from client
void ServerWindow::readMessage() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray data = socket->readAll();
    QImage image;

    //Data is image?
    if (image.loadFromData(data)) {
        //New qlabel to images
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(QPixmap::fromImage(image.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        //Pos of classic image
        imageLabel->setGeometry(608, 218, 160, 160);
        imageLabel->show();
    } else if (data.startsWith("SCREENSHOT")) { //is it screenshot?
        QByteArray screenshotData = data.mid(10);
        QImage screenshotImage;
        if (screenshotImage.loadFromData(screenshotData)) {
           //new qlabel for ss
            QLabel *screenshotLabel = new QLabel(this);
            screenshotLabel->setPixmap(QPixmap::fromImage(screenshotImage.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            screenshotLabel->setGeometry(608, 450, 160, 160);
            screenshotLabel->show();

        }
    } else {
        QString serverMessage = QString::fromUtf8(data);
        messageLog->setTextColor(Qt::white);
        messageLog->append("Server: " + serverMessage);
    }
}


void ServerWindow::sendMessageToClient() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        for (QTcpSocket *socket : connectedSockets) {//Send message to all clients
            if (socket->state() == QAbstractSocket::ConnectedState) {//If connection is still active
                socket->write(message.toUtf8());//Send message
                socket->flush();//Flush socket
            }
        }
        messageLog->append("Sent message to client: " + message);//Log of sent message
        messageInput->clear();//Clear field after sending
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

void ServerWindow::clearChat() {
    messageLog->clear();
}

void ServerWindow::shareScreen() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QPixmap screenshot = screen->grabWindow(0);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (buffer.open(QIODevice::WriteOnly)) {
        if (screenshot.save(&buffer, "JPEG")) {
            // Wysyłaj do wszystkich połączonych socketów
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



