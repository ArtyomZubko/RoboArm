#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool serial_init();

private slots:

    void sliderMoved(int);

    void sendTimerSlot();

private:
    Ui::MainWindow *ui;

    QTimer sendTimer;
    QSerialPort serial;
};

#endif // MAINWINDOW_H
