#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "klient.h"
#include <QListWidget>
#include <QMessageBox>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(this,SIGNAL(wyrzucenie(MainWindow::powod)),this,SLOT(disconnect(MainWindow::powod)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//logowanie do serwera
void MainWindow::on_pushButton_4_clicked()
{

    //pobranie wpisanego adresu serwera
    QHostAddress *adresSerewera = new QHostAddress(ui->textEditSerwer->text());

    //podlaczenie do serwera
    socket->connectToHost(*adresSerewera, 1234);

    //czekanie 2 sekund na polaczenie
    socket->waitForConnected(2000);

    //jezeli polaczony to przesun widget na karte o indeksie 1
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    //jezeli nie to wyswietl blad
    else
    {
        QMessageBox::information(this, "UWAGA!", "Nie udało się połączyć z serwerem!");
    }
}


//wysylanie wiadomosci
void MainWindow::on_pushButton_clicked()
{
    //pobranie tekstu z text boxa
    QString wiadomosc = ui->textEditWiadomosc->toPlainText().trimmed();

    //jezeli wiadomosc nie jest pusta to zapisanie na socket wiadomosci w formacie utf8
    if(!wiadomosc.isEmpty())
    {
        socket->write(QString(wiadomosc + "\n").toUtf8());
    }

    ui->textEditWiadomosc->clear();

    ui->textEditWiadomosc->setFocus();
}

void MainWindow::readyRead()
{

    //nasluchiwanie wiadomosci od serwera
    while(socket->canReadLine())
    {

        //deklaracja regexow
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QRegExp messageRegex("^([^:]+):(.*)$");
        QRegExp usersRegex("^/uzytkownicy:(.*)$");
        QRegExp serwerWylaczonyRegex("^/serwerwylaczony(.*)$");
        QRegExp zablokowanyRegex("^/zablokowany(.*)$");
        QRegExp wyrzuconyRegex("^/wyrzucony(.*)$");


        //dodawanie nazw uzytkownikow do listy uzytkownikow
        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            ui->listWidgetUzytkownicy->clear();
            foreach(QString user, users)
                new QListWidgetItem(user, ui->listWidgetUzytkownicy);
        }
        //wyswietlanie odebranej wiadomosci
        else if(messageRegex.indexIn(line) != -1)
        {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            ui->textBrowserRozmowa->append("<b>" + user + "</b>: " + message);
        }
        //obsluga rozlaczenia dookonanego przez serwer
        //wylaczenie serwera
        else if(serwerWylaczonyRegex.indexIn(line)!= -1)
        {
            emit wyrzucenie(MainWindow::powod::serverStop);
        }
        //zablokowanie uzytkownika przez administratora
        else if(zablokowanyRegex.indexIn(line)!= -1)
        {
            emit wyrzucenie(MainWindow::powod::block);
        }
        //wyrzucenie uzytkownika przez administratora
        else if(wyrzuconyRegex.indexIn(line)!= -1)
        {
            emit wyrzucenie(MainWindow::powod::kick);
        }

    }
}


//zgloszenie serwerowi dolaczenia klienta
void MainWindow::connected()
{
    //zapisanie na socket wiadomosci o dolaczeniu
    ui->stackedWidget->setCurrentIndex(1);
    socket->write(QString("/uzytkownik:" + ui->textEditNazwaUzytkownika->text() + "\n").toUtf8());
}


//rozlaczenie od serwera
void MainWindow::disconnect(MainWindow::powod e)
{
    //wyczyszczenie pola wiadomosci
    ui->textBrowserRozmowa->clear();

    //odlaczenie socketa od serwera
    socket->disconnectFromHost();
    ui->stackedWidget->setCurrentIndex(0);

    //obsluga powodow rozlaczenia
    if (e == MainWindow::powod::block){
        QMessageBox::information(this, "UWAGA!", "Zostałeś zablokowany przez administratora");
    } else if (e == MainWindow::powod::kick){
        QMessageBox::information(this, "UWAGA!", "Zostałeś wyrzucony przez administratora");
    } else if (e == MainWindow::powod::serverStop){
        QMessageBox::information(this, "UWAGA!", "Serwer został wyłączony");
    }
}


//wylogowanie reczne uzytkownika
void MainWindow::on_pushButtonWyloguj_clicked()
{
    //odlaczenie socketa od hosta
    socket->disconnectFromHost();
    //przesuniecie widgeta na index 0
    ui->stackedWidget->setCurrentIndex(0);
    //wyczyszczenie pola rozmowy
    ui->textBrowserRozmowa->clear();

}
