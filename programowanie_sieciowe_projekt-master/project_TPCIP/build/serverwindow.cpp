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
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender()); // Get the socket that sent the signal
    if (!socket) {
        return; // If the socket is null, exit the function
    }

    QByteArray data = socket->readAll(); // Read all data from the socket
    QImage image;

    if (image.loadFromData(data)) { // If data is an image
        if (image.loadFromData(data)) { //If data is an image
            //Find all QLabels except added frames
            QList<QLabel *> labels = findChildren<QLabel *>();
            for (QLabel *label : labels) {
                //Is frame?
                if (label->geometry() != QRect(600, 210, 180, 180) &&
                    label->geometry() != QRect(600, 450, 180, 180)) {
                    delete label;//Delete all exept for frame
                }
            }
        }
        // Scale image to 160 x 160
        QImage scaledImage = image.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        // Display picture
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
        imageLabel->setGeometry(608, 218, 160, 160); // Set label size to 160x160
        imageLabel->show();
        messageLog->append("Received an image from client."); // Log that an image was received
    } else {
        QString serverMessage = QString::fromUtf8(data); // If data is text
        messageLog->setTextColor(Qt::white); // Set message color to white
        messageLog->append("Server: " + serverMessage); // Log the server message
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


