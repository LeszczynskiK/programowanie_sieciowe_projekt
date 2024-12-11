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
    statusLabel->setGeometry(220, 10, 500, 50);

    //Initialize message input
    messageInput = new QLineEdit(this);
    messageInput->setFont(font);
    messageInput->setGeometry(10, 90, 400, 50);

    //Initialize buttons
    sendButton = new QPushButton("Send", this);
    sendButton->setFont(font);
    sendButton->setGeometry(420, 100, 120, 40);

    connectButton = new QPushButton("Connect...", this);
    connectButton->setFont(font);
    connectButton->setGeometry(550, 100, 150, 40); //Add geometry for the connect button

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
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ClientWindow::toggleScreenshotSending);

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

    screenshotTimer = new QTimer(this);
    connect(screenshotTimer,&QTimer::timeout, this, &ClientWindow::shareScreen);
    screenshotTimer->setInterval(100);
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
        connect(socket, &QTcpSocket::errorOccurred, this, &ClientWindow::handleSocketError);
    } else {
        statusLabel->setText("Please enter a valid IP address.");
    }
}

void ClientWindow::sendMessage() {//Send message to server
    QString message = messageInput->text();
    if (!message.isEmpty()) {
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

    if (data.isEmpty()) {
        qDebug() << "No data received.";
        return;
    }

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

    image.save(&buffer, "JPEG",60); // Save as jpeg60 is compression quality (0-100max)

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

void ClientWindow::onFullScreenScreenshotLabelDestroyed() {
    fullScreenScreenshotLabel = nullptr;
}

void ClientWindow::onFullImageLabelDestroyed() {
    fullScreenImageLabel = nullptr;
}

void ClientWindow::showFullScreenImage() {//Full size of image display in window
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
    QPixmap scaledPixmapImage = QPixmap::fromImage(receivedImage.scaled(960, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenImageLabel->setPixmap(scaledPixmapImage);

    fullScreenImageLabel->setWindowTitle("Image Preview");
    fullScreenImageLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenImageLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenImageLabel->show();

    //set nullptr after QLabel closing
    connect(fullScreenImageLabel, &QLabel::destroyed, this, &ClientWindow::onFullImageLabelDestroyed);
}

void ClientWindow::showFullScreenShare() {//Full size of screenshot display
    if (receivedScreenshot.isNull()) {
        return;//Img exist?
    }

    if (fullScreenScreenshotLabel) {//if photo exist, close old one
        fullScreenScreenshotLabel->close();
        fullScreenScreenshotLabel=nullptr;
    }

    //create new QLabel
    fullScreenScreenshotLabel = new QLabel;
    fullScreenScreenshotLabel->setFixedSize(960, 960);

    //Scale screenshot
    QPixmap scaledPixmapScreenshot = QPixmap::fromImage(receivedScreenshot.scaled(960, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fullScreenScreenshotLabel->setPixmap(scaledPixmapScreenshot);

    fullScreenScreenshotLabel->setWindowTitle("Screenshot Preview");
    fullScreenScreenshotLabel->setAttribute(Qt::WA_DeleteOnClose);//close and delete from memory
    fullScreenScreenshotLabel->setWindowFlags(Qt::Window);//Window type
    fullScreenScreenshotLabel->show();

    //set nullptr after QLabel closing
    connect(fullScreenScreenshotLabel, &QLabel::destroyed, this, &ClientWindow::onFullScreenScreenshotLabelDestroyed);
}


void ClientWindow::handleSocketError() {
    //Read error from socket
    QAbstractSocket::SocketError socketError = socket->error();

    //Display type of error
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        statusLabel->setText("Connection refused. Please check the server IP.");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        statusLabel->setText("Remote host closed the connection.");
        break;
    case QAbstractSocket::HostNotFoundError:
        statusLabel->setText("Host not found. Please check the IP address.");
        break;
    case QAbstractSocket::SocketAccessError:
        statusLabel->setText("Socket access error. Check your permissions.");
        break;
    case QAbstractSocket::SocketTimeoutError:
        statusLabel->setText("Connection timeout. Please try again.");
        break;
    case QAbstractSocket::NetworkError:
        statusLabel->setText("Network error. Check your connection.");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        statusLabel->setText("Address not available.");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        statusLabel->setText("Unsupported operation.");
        break;
    default:
        statusLabel->setText("An unknown error occurred: " + socket->errorString());
        break;
    }

    //Display on log
    messageLog->append("Error: " + socket->errorString());
}

void ClientWindow::toggleScreenshotSending() {
    sendingScreenshots = !sendingScreenshots;//Change flag(on/off screenshot sending)

    if (sendingScreenshots) {
        screenshotTimer->start();//Turn on timer
    } else {
        screenshotTimer->stop();//Stop timer
    }
}
