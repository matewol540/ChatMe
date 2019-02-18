#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include "serwer.h"
#include <QtDebug>
//Konstruktor i destruktor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){

    serwer = new Serwer();
    connect(serwer, SIGNAL(uzytkownikDolaczyl(QString)), this, SLOT(on_uzytkownikDolaczyl(QString)));
    connect(serwer,SIGNAL(uzytkownikWyszedl(QString)),this,SLOT(on_uzytkownikWyszedl(QString)));
    ui->setupUi(this);
    //Wyłączenie wszystkich przycisków na starcie serwera
    ui->pushButtonStopSerwera->setEnabled(false);
    ui->pushButtonZablokuj->setEnabled(false);
    ui->pushButtonUsun->setEnabled(false);
    ui->pushButtonOdblokuj->setEnabled(false);
    //Style dla włączonego przycisku
    ui->pushButtonStartSerwera->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");

    //Ustawienie tła aplikacji
    QPixmap bkgnd("bg.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //Wczytanie listy zablokowanych z pliku tekstowego
    listaZablokowanych = new QFile("zablokowani.txt");
    QVector<QString> zablokowani;

    if (listaZablokowanych->open(QIODevice::ReadOnly))
    {
       QTextStream in(listaZablokowanych);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          zablokowani.push_front(line);
          ui->listWidgetZablokowani->addItem(line);
       }
       listaZablokowanych->close();
       QVector<QHostAddress> *zablokowaniQHostsAddress = new QVector<QHostAddress>();
       foreach(QString s, zablokowani)
       {
           zablokowaniQHostsAddress->push_front(*new QHostAddress(s));
       }
       serwer->setZablokowani(*zablokowaniQHostsAddress);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
    delete serwer;
}
//Obluga sygnalow z przyciskow
void MainWindow::on_pushButtonZablokuj_clicked()
{
    //tworzenie tymczasowej kopii mapy uzytkownikow i socketow
    QMap<QTcpSocket*,QString>uzytkownicy = serwer->getUzytkownicy();

    //pobranie nicku do zbanowania z listy
    QString nickDoZbanowania = ui->listWidgetUzytkownicy->currentItem()->text();

    //utworzenie socketu do zbanowania na podstawie nicku
    QTcpSocket *socketDoZbanowania = uzytkownicy.key(nickDoZbanowania);

    //wyciagnniecie "surowego" ip z socketa
    QString ipString = socketDoZbanowania->peerAddress().toString();

    //usuwanie z listy z UI
    delete ui->listWidgetUzytkownicy->currentItem();

    //Tymczasowy vector zablokowanych już IP z pliku
    QVector<QString> zablokowani;

    //tymczasowy vector socketow zablokowanych
    QVector<QHostAddress> *zablokowaniQHostsAddress = new QVector<QHostAddress>();

    //otwarcie
    listaZablokowanych = new QFile("zablokowani.txt");
    listaZablokowanych->setFileName("zablokowani.txt");

    //Aktualizowanie listy zablokowanych po kliknieciu w przycisk
    QRegExp ipRegex("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
    QStringList ip;
    if (listaZablokowanych->open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream inout(listaZablokowanych);

        ipRegex.indexIn(ipString);
        ip = ipRegex.capturedTexts();
        inout << ip.at(0);
        zablokowani.push_front(ip.at(0));
        listaZablokowanych->close();
    }

    //Aktualizacja listy widocznej w serwerze
    ui->listWidgetZablokowani->addItem(ip.at(0));
    foreach(QString s, zablokowani){
        zablokowaniQHostsAddress->push_front(*new QHostAddress(s));
        }

    socketDoZbanowania->write(QString("/zablokowany\n").toUtf8());

    serwer->setZablokowani(*zablokowaniQHostsAddress);

    //Aktualizacja przyciskow
    ui->pushButtonUsun->setEnabled(false);
    ui->pushButtonUsun->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
    ui->pushButtonZablokuj->setEnabled(false);
    ui->pushButtonZablokuj->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
}
void MainWindow::on_pushButtonStartSerwera_clicked()
{

    //wyczyszczenie listy uzytkownikaow
    ui->listWidgetUzytkownicy->clear();

    //ustawienie flagi sukces w zaleznosci od powodzenia rozpoczecia nasluchiwania
    //jezeli sie nie uda to wyswietl ostrzezenie
    bool sukces = serwer->listen(QHostAddress::Any, 1234);
    if(!sukces)
    {
        QMessageBox::information(this, "OSTRZEZENIE", "NIE UDALO SIE NAWIAZAC POLACZENIA");
    }

    //ustawienie przyciskow
    ui->pushButtonStartSerwera->setEnabled(false);
    ui->pushButtonStopSerwera->setEnabled(true);

    ui->pushButtonStartSerwera->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
    ui->pushButtonStopSerwera->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");

    ui->labelSerwerInfo->setText("WŁĄCZONY");
    ui->labelSerwerInfo->setStyleSheet("background-color:green;font-size:56px;border-radius:20px;qproperty-alignment: AlignCenter;");
}
void MainWindow::on_pushButtonStopSerwera_clicked()
{
    //Pobranie listy uzytkownikow z mapy
    QSet<QTcpSocket*> klienci = serwer->getKlienci();

    //Rozeslanie wiadomosci do wszystkich klientow, ze serwer zostal wylaczony
    foreach(QTcpSocket *s, klienci)
    {
        s->write(QString("/serwerwylaczony\n").toUtf8());        
    }
    serwer->czyscUzytkownicy();
    serwer->czyscKlienci();

    ui->listWidgetUzytkownicy->clear();

    //Obluga styli w ui
    ui->pushButtonStopSerwera->setEnabled(false);
    ui->pushButtonStartSerwera->setEnabled(true);
    ui->pushButtonStartSerwera->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");
    ui->pushButtonStopSerwera->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
    ui->labelSerwerInfo->setText("WYŁĄCZONY");
    ui->labelSerwerInfo->setStyleSheet("background-color:red;font-size:56px;border-radius:20px;qproperty-alignment: AlignCenter;");
    serwer->close();

}
void MainWindow::on_pushButtonOdblokuj_clicked()
{
    //Otworzenie sturmienia pliku z zablokowanymi uzytkownikami
    listaZablokowanych = new QFile("zablokowani.txt");
    QString wybraneIp = ui->listWidgetZablokowani->currentItem()->text();
    QVector<QString> zablokowani;

    //Wczytywanie zawartosci pliku
    if (listaZablokowanych->open(QIODevice::ReadOnly)){
       QTextStream in(listaZablokowanych);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          zablokowani.push_front(line);
       }
       listaZablokowanych->close();
    }
    //Usuwanie wpisu z plku
    if(zablokowani.contains(wybraneIp))
    {
        zablokowani.removeAll(wybraneIp);
    }
    //Wpisanie do pliku nowej listy zablokowanych uzytkownikow
    if (listaZablokowanych->open(QIODevice::WriteOnly | QIODevice::Text)){
       QTextStream inout(listaZablokowanych);
        foreach(QString s, zablokowani)
        {
            inout << s;
        }
        listaZablokowanych->close();
    }

    QVector<QHostAddress> *zablokowaniQHostsAddress = new QVector<QHostAddress>();

    foreach(QString s, zablokowani)
    {
        zablokowaniQHostsAddress->push_front(*new QHostAddress(s));
    }

    serwer->setZablokowani(*zablokowaniQHostsAddress);

    delete ui->listWidgetZablokowani->currentItem();

    //Style przyciskow
    ui->pushButtonOdblokuj->setEnabled(false);
    ui->pushButtonOdblokuj->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
}
void MainWindow::on_pushButtonUsun_clicked()
{
    //Pobranie wybranego elementu z listy
    QString uzytkownik = ui->listWidgetUzytkownicy->currentItem()->text();
    //Pobranie mapy uzytkownikow
    QMap<QTcpSocket*,QString>uzytkownicy = serwer->getUzytkownicy();
    //Pobranie adresu do usuniecia
    QTcpSocket *socketDoUsuniecia = uzytkownicy.key(uzytkownik);
    //Wyslanie wiadomosci do uzytkownika ze zostal wyrzucony
    socketDoUsuniecia->write(QString("/wyrzucony\n").toUtf8());
    //Obsluga styli przyciskow
    ui->pushButtonUsun->setEnabled(false);
    ui->pushButtonUsun->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
    ui->pushButtonZablokuj->setEnabled(false);
    ui->pushButtonZablokuj->setStyleSheet("border-radius:5px;background-color:#82868c;border: 2px solid black;color: black;");
}
//Oblusga polaczen przychodzacych i odchodzacych
void MainWindow::on_uzytkownikDolaczyl(QString uzytkownik)
{
    //Wpisanie do listy nowo dolaczonego uzytkownika
    ui->listWidgetUzytkownicy->addItem(uzytkownik);
    qDebug() << "debug dolaczyl:" + uzytkownik;
}
void MainWindow::on_uzytkownikWyszedl(QString uzytkownik){
    QString user = "";
    int i = 0;
    //Poszukiwanie indeksu uzytkownika ktory wyszedl

    for(int i = 0; i < ui->listWidgetUzytkownicy->count(); i++)
    {
        user = ui->listWidgetUzytkownicy->item(i)->text();
        if(user == uzytkownik)
        {
            ui->listWidgetUzytkownicy->takeItem(i);
        }
    }
}


//Obsluga list w ui
void MainWindow::on_listWidgetUzytkownicy_itemPressed(QListWidgetItem *item)
{
    //Style przyciskow
    ui->pushButtonZablokuj->setEnabled(true);
    ui->pushButtonZablokuj->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");
    ui->pushButtonUsun->setEnabled(true);
    ui->pushButtonUsun->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");
}
void MainWindow::on_listWidgetZablokowani_itemPressed(QListWidgetItem *item)
{
    //Style przyciskow
    ui->pushButtonOdblokuj->setEnabled(true);
    ui->pushButtonOdblokuj->setStyleSheet("border-radius:5px;background-color:#c7c9cc;border: 2px solid black;color: black;");
}
