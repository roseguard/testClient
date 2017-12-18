#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QXmlStreamWriter>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QSettings>

#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    QVBoxLayout     *lay;
    QTextBrowser    *messageBox;

    QLabel          *messageLab;
    QLineEdit       *messageLine;

    QLabel          *commandsLab;
    QLineEdit       *commandsLine;

    QPushButton     *sendButt;

    QTcpSocket      *socket;

    QByteArray      login;
    int             getMessagesCount = 0;
    static QSettings       settings;
public slots:
    void sendMessage();
    void initMess(QByteArray message, QByteArray command);
    void getMessReply();
};

#endif // WIDGET_H
