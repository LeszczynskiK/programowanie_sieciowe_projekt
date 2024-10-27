#include "serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent) {
    setFixedSize(860, 860);//Set window size
    setWindowTitle("Server Window");

    QList<QTcpSocket*> connectedSockets;//list of connected clients

    QFont font;
    font.setPointSize(12);//Font size - beginning initialization

    //Server status label
    statusLabel = new QLabel("Server is not running", this);
    statusLabel->setGeometry(10, 10, 600, 200);
    statusLabel->setFont(font);

    //Message log field
    messageLog = new QTextEdit(this);
    messageLog->setReadOnly(true);
    messageLog->setGeometry(120, 120, 540, 540);
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
}

//Start button click
void ServerWindow::onStartButtonClicked() {
    if (tcpServer->listen(QHostAddress::Any, 12351)) {
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

//Read message from client
void ServerWindow::readMessage() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QByteArray data = socket->readAll(); // Read data from the socket
        messageLog->append("Received message: " + data); // Log received message
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
    // Otwórz dialog do wyboru pliku
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (fileName.isEmpty()) {
        return; // Jeśli użytkownik nie wybrał pliku, zakończ
    }

    QImage image(fileName); // Użyj wybranego pliku
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG"); // Zapisz obraz w formacie JPEG

    // Wysyłaj obraz do wszystkich podłączonych gniazd
    for (QTcpSocket *socket : connectedSockets) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(byteArray); // Wyślij dane
            socket->flush(); // Wypchnij dane
        }
    }
}

\
