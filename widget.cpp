#include "widget.h"

QSettings Widget::settings("sets.ini", QSettings::IniFormat);

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    lay = new QVBoxLayout(this);

    messageBox = new QTextBrowser(this);

    messageLab = new QLabel(tr("Message"), this);
    messageLine = new QLineEdit(this);

    commandsLab = new QLabel(tr("Commands"), this);
    commandsLine = new QLineEdit(this);

    sendButt = new QPushButton(tr("Send"), this);
    connect(sendButt, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));

    lay->addWidget(messageBox);

    lay->addWidget(messageLab);
    lay->addWidget(messageLine);

    lay->addWidget(commandsLab);
    lay->addWidget(commandsLine);

    lay->addWidget(sendButt);

    socket = new QTcpSocket(this);

    settings.beginGroup("User");
    login = settings.value("login").toString().toUtf8();
    settings.endGroup();

    settings.beginGroup("Options");
    getMessagesCount = settings.value("messages").toInt();
    settings.endGroup();

    settings.beginGroup("Connection");
    socket->connectToHost(settings.value("address").toString(), settings.value("port").toInt());
    settings.endGroup();

    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessReply()));

    if(socket->waitForConnected())
    {
        initMess("", "getmessages " + QByteArray::number(getMessagesCount));
    }
}

Widget::~Widget()
{

}

void Widget::sendMessage()
{
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

void Widget::initMess(QByteArray message, QByteArray command)
{
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
    qDebug() << "NEW REPLY";
    QMap<QByteArray, QByteArray> httpRequestParams;
    int packetSize = 0;
    while(socket->bytesAvailable() > 0)     // парсим параметры запроса
    {
        QByteArray tempLine = socket->readLine();
        qDebug() << tempLine;
        if(tempLine=="\r\n")   // конец параметрам запроса
            break;
        QList<QByteArray> tempHttpParamList = QString(tempLine).remove('\r').remove('\n').toUtf8().split(':');
        httpRequestParams[tempHttpParamList.first()] = tempHttpParamList.last();
    }
    packetSize = httpRequestParams["Content-Length"].toInt();

    if(packetSize==0) // если запрос не передал размер данных
        return;

    QByteArray xmlArray;
    while(xmlArray.length() < packetSize)
    {
        if(socket->bytesAvailable()>0)
        {
            QByteArray newPart = socket->read(packetSize-xmlArray.length());
            qDebug() << newPart;
            xmlArray.append(newPart);
        }
        else
        {
            socket->waitForReadyRead();
        }
    }

    QBuffer buff(&xmlArray);        // создаем буфер для работы с xml reader
    buff.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&buff);

    QByteArray messageLogin = "";
    while(!reader.atEnd()) // обрабатываем xml
    {
        reader.readNext();
#ifdef __DEBUG
        qDebug() << reader.name();
#endif
        if(reader.name()=="Login")
        {
            messageLogin = reader.readElementText().toUtf8();
        }
        if(reader.name()=="Message")
        {
            messageBox->append(messageLogin + " : " + reader.readElementText());
        }
        if(reader.name()=="Command")
        {
            messageBox->append("Server : " + reader.readElementText());
        }
    }

    buff.close();
}
