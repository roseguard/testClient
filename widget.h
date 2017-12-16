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

    QLabel          *loginLab;
    QLineEdit       *loginLine;

    QLabel          *messageLab;
    QLineEdit       *messageLine;

    QLabel          *commandsLab;
    QLineEdit       *commandsLine;

    QPushButton     *sendButt;

    QTcpSocket      *socket;
public slots:
    void sendMessage();
    void getMessReply();
};

#endif // WIDGET_H
