#ifndef KLIENT_H
#define KLIENT_H

#include <QObject>
#include <QTcpSocket>
class Klient : public QObject
{
    Q_OBJECT
public:
    explicit Klient(QObject *parent = nullptr);
    void doConnect();
signals:

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
private:
};

#endif // KLIENT_H
