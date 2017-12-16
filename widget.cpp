#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    lay = new QVBoxLayout(this);

    messageBox = new QTextBrowser(this);

    loginLab = new QLabel(tr("Login"), this);
    loginLine = new QLineEdit(this);

    messageLab = new QLabel(tr("Message"), this);
    messageLine = new QLineEdit(this);

    commandsLab = new QLabel(tr("Commands"), this);
    commandsLine = new QLineEdit(this);

    sendButt = new QPushButton(tr("Send"), this);
    connect(sendButt, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));


    lay->addWidget(messageBox);

    lay->addWidget(loginLab);
    lay->addWidget(loginLine);

    lay->addWidget(messageLab);
    lay->addWidget(messageLine);

    lay->addWidget(commandsLab);
    lay->addWidget(commandsLine);

    lay->addWidget(sendButt);

    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 21034);
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessReply()));

}

Widget::~Widget()
{

}

void Widget::sendMessage()
{
    QByteArray login = loginLine->text().toUtf8();
    QByteArray message = messageLine->text().toUtf8();
    QByteArray command = commandsLine->text().toUtf8();

    QByteArray requestArray;
    QBuffer buff(&requestArray);
    buff.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&buff);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("REQUEST");
    writer.writeStartElement("MESSAGES");

    writer.writeStartElement("MESSAGE");

    if(!login.isEmpty())
        writer.writeTextElement("Login", login);
    if(!message.isEmpty())
        writer.writeTextElement("Message", message);
    if(!command.isEmpty())
        writer.writeTextElement("Command", command);

    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndElement();

    writer.writeEndDocument();

    buff.close();

    qDebug() << requestArray;

    socket->write("HTTP/1.0 200 Ok\r\n");
    socket->write("Content-Type: application/xml\r\n");
    socket->write("Connection: Keep-Alive\r\n");
    socket->write("Content-Length: +" + QByteArray::number(requestArray.size()) + "\r\n");
    socket->write("\r\n");
    socket->write(requestArray);
    socket->flush();
}

void Widget::getMessReply()
{
    messageBox->append(socket->readAll());
}
