#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QtSerialPort/QSerialPort>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalSlider->setRange(0,180);
    ui->verticalSlider_2->setRange(0,180);
    ui->verticalSlider_3->setRange(0,180);
    ui->verticalSlider_4->setRange(0,180);

    connect(ui->verticalSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_2, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_3, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_4, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));

    sendTimer.setInterval(30);
    sendTimer.setSingleShot(false);
    connect(&sendTimer, SIGNAL(timeout()), this, SLOT(sendTimerSlot()));

    if(serial_init()){
        sendTimer.start();
    }
    else{
        qDebug() << "Error opening serial port, timer not started";
    }
}

MainWindow::~MainWindow()
{
    serial.close();
    delete ui;
}

bool MainWindow::serial_init()
{
    serial.setPortName("COM6");
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    if(serial.open(QSerialPort::WriteOnly)){
        return true;
    }
    else {
        serial.close();
        return false;
    }
}

void MainWindow::sliderMoved(int)
{
    QSlider *slider = (QSlider*)sender();

}

void MainWindow::sendTimerSlot()
{
    unsigned char val[4];
    memset(val, 0, sizeof(unsigned char)*4);

    val[0] = (unsigned char)ui->verticalSlider->value();
    val[1] = (unsigned char)ui->verticalSlider_2->value();
    val[2] = (unsigned char)ui->verticalSlider_3->value();
    val[3] = (unsigned char)ui->verticalSlider_4->value();

    //qDebug() << "timer slot!!! sending:" << val[0] << val[1] << val[2] << val[3];

    QByteArray outPacket;
    outPacket.append('*');
    outPacket.append(val[0]);
    outPacket.append(val[1]);
    outPacket.append(val[2]);
    outPacket.append(val[3]);
    outPacket.append('#');

    qDebug() << outPacket;

    if(serial.isOpen()){
        serial.write(outPacket);
    }
}


