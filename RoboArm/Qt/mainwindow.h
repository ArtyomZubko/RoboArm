#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

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

    void setStartPos();

    void showPorts();

    bool recState;

    QVector<QByteArray> recordedPackets;

    QByteArray packetForming();

    int sendedPacketsCount;


private slots:

    void sliderMoved(int);

    void sendTimerSlot();

    void on_pushButton_clicked();

    void on_recordStartButton_clicked();

    void recordedSendTimerSlot();

    void on_recordEndButton_clicked();

    void on_playButton_clicked();

    void menuTriggered();



    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;

    QTimer sendTimer,recordedSendTimer;

    QSerialPort serial;

};

#endif // MAINWINDOW_H
