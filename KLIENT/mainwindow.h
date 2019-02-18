#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    enum powod{block,kick,serverStop};


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();

    void readyRead();
    void connected();
    void on_pushButtonWyloguj_clicked();
public slots:
    void disconnect(MainWindow::powod);

signals:
    void wyrzucenie(MainWindow::powod);
};

#endif // MAINWINDOW_H
