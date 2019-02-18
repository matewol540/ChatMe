#ifndef SERWER_H
#define SERWER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <qfile.h>

class Serwer : public QTcpServer
{
    Q_OBJECT
public:
    explicit Serwer(QObject *parent = nullptr);
    QMap<QTcpSocket*,QString> getUzytkownicy();
    QSet<QTcpSocket*> getKlienci();
    void czyscUzytkownicy();
    void czyscKlienci();
    void odlaczWszystkichUzytkownikow();
    void setZablokowani(QVector<QHostAddress>);
private slots:
    void readyRead();
    void disconnected();
    void sendUserList();
signals:
     void uzytkownikWyszedl(QString uzytkownik);
     void uzytkownikDolaczyl(QString uzytkownik);
protected:
    void incomingConnection(int socketfd);

private:
    QSet<QTcpSocket*> klienci;
    QMap<QTcpSocket*,QString> uzytkownicy;
    QVector<QHostAddress> *zablokowaniUzytkownicy;

};

#endif // SERWER_H
