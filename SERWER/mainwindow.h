#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QListWidget>
#include "serwer.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
        Serwer *serwer;
    Ui::MainWindow *ui;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
private slots:
    void on_pushButtonZablokuj_clicked();

    void on_pushButtonStartSerwera_clicked();

    void on_pushButtonStopSerwera_clicked();

    void on_pushButtonOdblokuj_clicked();

    void on_listWidgetUzytkownicy_itemPressed(QListWidgetItem *item);

    void on_pushButtonUsun_clicked();

    void on_listWidgetZablokowani_itemPressed(QListWidgetItem *item);

public slots:
    void on_uzytkownikDolaczyl(QString uzytkownik);
    void on_uzytkownikWyszedl(QString uzytkownik);

private:
    QFile *listaZablokowanych;
};

#endif // MAINWINDOW_H
