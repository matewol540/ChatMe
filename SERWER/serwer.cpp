#include "serwer.h"
#include "mainwindow.h"
Serwer::Serwer(QObject *parent) : QTcpServer(parent)
{

}

//metoda odpowiedzialna za obsluge nadchodzacych polaczen
void Serwer::incomingConnection(int socketfd)
{
    //utworzenie nowego klienta, ustawienie socket descriptora i wsadzenie klienta do kolekcji klientow
    QTcpSocket *klient = new QTcpSocket(this);
    klient->setSocketDescriptor(socketfd);
    klienci.insert(klient);


    //sprawdzenie czy klient znajduje sie na liscie zbanowanych
    QRegExp ipRegex("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
    QStringList ipList;
    QString hostString = klient->peerAddress().toString();
    ipRegex.indexIn(hostString);
    ipList = ipRegex.capturedTexts();
    QString ip = ipList.at(0);

    QHostAddress *sprawdzanyUzytkownik = new QHostAddress(ip);
    if(!zablokowaniUzytkownicy->isEmpty())
    {
        //jezeli ip znajduje sie na liscie zablokowanych to odeslij mu wiadomosc o braku dostepu
            if(zablokowaniUzytkownicy->contains(*sprawdzanyUzytkownik))
            {
                klient->write(QString("/zablokowany\n").toUtf8());
            }
    }



    connect(klient, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(klient, SIGNAL(disconnected()), this, SLOT(disconnected()));
}


//metoda obslugujaca odczytywanie
void Serwer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    //odczyt nadchadzacych danych z socketa w petli
    while(client->canReadLine())
    {
        //zapisanie do zmiennej zawartosci wiadomosci
        QString line = QString::fromUtf8(client->readLine()).trimmed();

        //sprawdzenie regexem jaka "komenda" zostala przyslana

            //obsluga dolaczenia nowego uzytkownika:
        QRegExp meRegex("^/uzytkownik:(.*)$");

        if(meRegex.indexIn(line) != -1)
        {
            QString uzytkownik = meRegex.cap(1);
            uzytkownicy[client] = uzytkownik;
            foreach(QTcpSocket *client, klienci)
                client->write(QString("SERWER:" + uzytkownik + " DOŁĄCZYŁ.\n").toUtf8());
            emit uzytkownikDolaczyl(uzytkownik);
            sendUserList();
        }
        else if(uzytkownicy.contains(client))
        {
           QString wiadomosc = line;
           QString uzytkownik = uzytkownicy[client];
           foreach(QTcpSocket *otherClient, klienci)
                otherClient->write(QString(uzytkownik + ":" + wiadomosc + "\n").toUtf8());
        }
        if(!zablokowaniUzytkownicy->isEmpty())
        {
            foreach(QTcpSocket *klient, klienci)
            {
                if(zablokowaniUzytkownicy->contains(klient->peerAddress()))
                {
                    klient->write(QString("zablokowany\n").toUtf8());
                }
            }
        }
    }
}


//obsluga rozlaczenia sie uzytkownika
void Serwer::disconnected()
{

    //usuniecie z kolekcji uzytkownika i odpowiadajacego mu socketa
    QTcpSocket *client = (QTcpSocket*)sender();
    klienci.remove(client);
    QString user = uzytkownicy[client];
    uzytkownicy.remove(client);

    //rozeslanie do wszystkich uzytkownikow wiadomosci o wylogowaniu sie uzytkownika
    sendUserList();
    foreach(QTcpSocket *client, klienci)
        client->write(QString("SERWER:" + user + " WYLOGOWAŁ SIĘ. \n").toUtf8());
    emit uzytkownikWyszedl(user);
}


// metoda wysylajaca wszystkim uzytkownikom aktualna liste uzytkownikow
void Serwer::sendUserList()
{
    QStringList listaUzytwkownikow;
    foreach(QString user, uzytkownicy.values())
        listaUzytwkownikow << user;
    foreach(QTcpSocket *client, klienci)
        client->write(QString("/uzytkownicy:" + listaUzytwkownikow.join(",") + "\n").toUtf8());

    qDebug() << this->getKlienci();
}


//metoda wysylajaca wszystkim uzytkownikom polecenie odlaczenia sie
//wykorzystywana przy zamykaniu recznym naslucchiwania serwera
void Serwer::odlaczWszystkichUzytkownikow()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    while(client->canReadLine())
    {
        foreach(QTcpSocket *client, klienci)
        {
            client->flush();
            client->write(QString("/serwerwylaczony\n").toUtf8());
        }
    }
}

QMap<QTcpSocket*,QString> Serwer::getUzytkownicy()
{
    return uzytkownicy;
}

QSet<QTcpSocket*> Serwer::getKlienci()
{
    return klienci;
}

void Serwer::setZablokowani(QVector<QHostAddress> zablokowaniParam)
{
    zablokowaniUzytkownicy = new QVector<QHostAddress>(zablokowaniParam);
}

void Serwer::czyscUzytkownicy()
{
    uzytkownicy.clear();
}


void Serwer::czyscKlienci()
{
    klienci.clear();
}

