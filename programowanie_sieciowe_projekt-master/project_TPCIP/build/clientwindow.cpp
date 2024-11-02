#include "clientwindow.h"



ClientWindow::ClientWindow(QWidget *parent) : QWidget(parent) {

    setWindowTitle("Client Window");

    const int x =860;//x window size
    const int y=860;//y window size

    setFixedSize(x, y);//Set window size
    //Background image
    background = QPixmap("/home/krzysiek89/Desktop/QT_aplikacje/Programowanie_sieciowe/programowanie_sieciowe_projekt-master/project_TPCIP/bck.jpg").scaled(x, y);


    QFont font;
    font.setPointSize(15);//Font size - beginning initialization

    //Initialize IP address input
    ipInput = new QLineEdit(this);
    ipInput->setGeometry(10, 10, 200, 50);
    ipInput->setFont(font);
    ipInput->setPlaceholderText("Enter server IP address");

    //Initialize status label
    statusLabel = new QLabel("Disconnected", this);
    statusLabel->setFont(font);
    statusLabel->setStyleSheet("color: yellow;");
    statusLabel->setGeometry(220, 10, 200, 50);

    //Initialize message input
    messageInput = new QLineEdit(this);
    messageInput->setFont(font);
    messageInput->setGeometry(10, 90, 400, 50);

    //Initialize buttons
    sendButton = new QPushButton("Send", this);
    sendButton->setFont(font);
    sendButton->setGeometry(420, 20, 120, 40);

    connectButton = new QPushButton("Connect...", this);
    connectButton->setFont(font);
    connectButton->setGeometry(550, 20, 150, 40); //Add geometry for the connect button

    //Connect signals and slots
    connect(connectButton, &QPushButton::clicked, this, &ClientWindow::connectToServer);
    connect(sendButton, &QPushButton::clicked, this, &ClientWindow::sendMessage);

    //Initialize socket
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &ClientWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::readMessage);

    //Messages from server
    messageLog = new QTextEdit(this);
    messageLog->setReadOnly(true);
    messageLog->setGeometry(10, 150, 540, 540);
    messageLog->setFont(font);

    //Image send button
    sendImageButton = new QPushButton("Send Image to Server", this);
    sendImageButton->setGeometry(10, 720, 200, 50);
    connect(sendImageButton, &QPushButton::clicked, this, &ClientWindow::sendImageToServer);

    //Initialize delete button
    QPushButton *deleteButton = new QPushButton("Clear Chat", this);
    deleteButton->setFont(font);
    deleteButton->setGeometry(250, 720, 120, 50); // Geometry for the delete button
    connect(deleteButton, &QPushButton::clicked, this, &ClientWindow::clearChat);

    //Desktop sharing
    QPushButton *sendScreenshotButton = new QPushButton("Send Screenshot", this);
    sendScreenshotButton->setGeometry(410, 720, 200, 50);
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ClientWindow::shareScreen);

    //Disconnect from server button
    disconnectButton = new QPushButton("Disconnect", this);
    disconnectButton->setFont(font);
    disconnectButton->setGeometry(650, 720, 200, 50); // Pozycja i rozmiar przycisku
    connect(disconnectButton, &QPushButton::clicked, this, &ClientWindow::disconnectFromServer);

    //Add frame based on QLabel
    QLabel *frame = new QLabel(this);//Frame 1
    frame->setGeometry(600, 210, 180, 180);//Pos and size
    frame->setStyleSheet("border: 2px solid grey;");//Style of frame

    QLabel *frame1 = new QLabel(this);//Frame 2
    frame1->setGeometry(600, 450, 180, 180);//Pos and size
    frame1->setStyleSheet("border: 2px solid grey;");//Style of frame

    screenshotLabel = new ClickableLabel(this);//Screenshot got
    connect(screenshotLabel, &ClickableLabel::clicked, this, &ClientWindow::showFullScreenShare);//Screenshot event

    imageLabel = new ClickableLabel(this);//Image got
    connect(imageLabel, &ClickableLabel::clicked, this, &ClientWindow::showFullScreenImage);//Image event
}

void ClientWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);//Background
    QWidget::paintEvent(event);
}

void ClientWindow::connectToServer() {
    QString ipAddress = ipInput->text();//Get the IP address from the input field
    if (!ipAddress.isEmpty()) {
        socket->connectToHost(ipAddress, 12353);//Attempt to connect to the server
    } else {
        statusLabel->setText("Please enter a valid IP address.");
    }
}

void ClientWindow::sendMessage() {//Send message to server
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        qDebug() << "Sending message:" << message;//Log messages
        socket->write(message.toUtf8());
        socket->flush();
        messageInput->clear();//Clear field after sent
    }
}


void ClientWindow::onConnected() {//Information about connection
    statusLabel->setText("Connected to server");
    sendMessage();//Send greeting
}

void ClientWindow::disconnectFromServer() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();//Disconnect
        socket->waitForDisconnected();//Wait for disconnection
        statusLabel->setText("Disconnected");//Actualise status
        messageLog->append("Disconnected from server.");
    }

    else
    {
        messageLog->append("Already disconnected.");//If acutally disconnected...
    }
}

void ClientWindow::readMessage() {//Read all type of messages
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray data = socket->readAll();
    QImage image;

    if (data == "forced_stop") {//if stop forced - change status label
        statusLabel->setText("Forced Stop"); // Update connection status
    }

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
        messageLog->setTextColor(Qt::red);
        messageLog->append("Server: " + serverMessage);
    }
}

void ClientWindow::sendImageToServer() {//Send img
    //Dialog to choose image file
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (fileName.isEmpty()) {
        return; // Finish if not taken
    }

    QImage image(fileName); // Use chosen file
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG"); // Save as jpeg

    if (socket->state() == QAbstractSocket::ConnectedState) { // Check if the socket is connected
        socket->write(byteArray); // Send the image to the server
        socket->flush(); // Push the data
    }
}

void ClientWindow::clearChat() {//Clear chat method
    messageLog->clear();
}

void ClientWindow::shareScreen() {//Send screenshot of desktop
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QPixmap screenshot = screen->grabWindow(0);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (buffer.open(QIODevice::WriteOnly)) {
        if (screenshot.save(&buffer, "JPEG")) {
            if (socket->state() == QAbstractSocket::ConnectedState) {
                // add prefix screenshod to make possible recoglise ss from classig image
                QByteArray dataToSend = "SCREENSHOT" + byteArray;
                socket->write(dataToSend);
                socket->flush();
            } else {
                messageLog->append("Socket not connected.");
            }
        } else {
            messageLog->append("Failed to save screenshot.");
        }
    } else {
        messageLog->append("Failed to open buffer.");
    }
}


void ClientWindow::showFullScreenImage() {//Full size of image display in window
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

void ClientWindow::showFullScreenShare() {//Full size of screenshot display
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
