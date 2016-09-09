#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    ui->setupUi(this);
    ui->verticalSlider->setRange(0,180);
    ui->verticalSlider_2->setRange(0,180);
    ui->verticalSlider_3->setRange(0,180);
    ui->verticalSlider_4->setRange(0,180);

    ui->progressBar->reset();

    setStartPos();

    recState = false;

    this->setWindowTitle("ArmControl");

    ui->filenameLable->setText("Сценарий не загружен");

    ui->recordEndButton->hide();

   // QWidget::setFixedSize(314,425);

    connect(ui->verticalSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_2, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_3, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(ui->verticalSlider_4, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));

    connect(ui->saveAction, SIGNAL(triggered()), this, SLOT(menuTriggered()));
    connect(ui->loadAction, SIGNAL(triggered()), this, SLOT(menuTriggered()));

    sendTimer.setInterval(15);
    sendTimer.setSingleShot(false);
    connect(&sendTimer, SIGNAL(timeout()), this, SLOT(sendTimerSlot())); 

    recordedSendTimer.setInterval(15);
    recordedSendTimer.setSingleShot(false);
    connect(&recordedSendTimer, SIGNAL(timeout()), this, SLOT(recordedSendTimerSlot()));

    showPorts();

    if(serial_init()){
        sendTimer.start();
        ui->connectLabel->setText("Связь установлена");
    }
    else{
        ui->connectLabel->setText("Связь потеряна");
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
    serial.setPortName(ui->comboBox->currentText());
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


void MainWindow::setStartPos() //приведение в стартовую позицию
{
    ui->verticalSlider->setValue(0);
    ui->verticalSlider_2->setValue(152); // начальное пoлoжение
    ui->verticalSlider_3->setValue(50);
    ui->verticalSlider_4->setValue(79);
}

void MainWindow::showPorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (int i = 0; i <= ports.length() - 1; i++){
        ui->comboBox->addItem(ports[i].portName());
    }

}

void MainWindow::sliderMoved(int) //слот движения слайдеров
{
    //QSlider *slider = (QSlider*)sender();


    QByteArray a = packetForming();
    if (recState){
        recordedPackets.append(a);
    }

}

void MainWindow::sendTimerSlot() //фоновая рассылка пакетов
{   
    if(serial.isOpen()){
        serial.write(packetForming());
        ui->connectLabel->setText("Связь установлена");
    }else
         ui->connectLabel->setText("Связь потеряна");
}



void MainWindow::on_pushButton_clicked()
{
    setStartPos();

}

void MainWindow::on_recordStartButton_clicked() //кнопка старта записи
{
    if(!recState)
    {
        recordedPackets.clear();
        recState = true;
        ui->recordStartButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255);"));
        ui->recordStartButton->setText("В режиме обучения!");
        ui->recordEndButton->show();
    }
}



QByteArray MainWindow::packetForming() //ф-я формирования пакета
{
    unsigned char val[4];
    memset(val, 0, sizeof(unsigned char)*4);

    val[0] = (unsigned char)ui->verticalSlider->value();
    val[1] = (unsigned char)ui->verticalSlider_2->value();
    val[2] = (unsigned char)ui->verticalSlider_3->value();
    val[3] = (unsigned char)ui->verticalSlider_4->value();



    QByteArray outPacket;

    outPacket.append('*');
    outPacket.append(val[0]);
    outPacket.append(val[1]);
    outPacket.append(val[2]);
    outPacket.append(val[3]);
    outPacket.append('#');

    qDebug() << outPacket;

    return outPacket;

}




void MainWindow::on_recordEndButton_clicked() //кнопка окончания записи
{
    recState = false;
    ui->recordStartButton->setStyleSheet(QString::fromUtf8("background-color: QPalette::Button;"));
    ui->recordStartButton->setText("Начать обучение");
    ui->filenameLable->setText("Пользовательский сценарий");
    ui->recordEndButton->hide();

}



void MainWindow::on_playButton_clicked() //проигрывание сценария
{
    sendedPacketsCount = 0;
    sendTimer.stop();

    setStartPos();
    serial.write(packetForming());

    ui->progressBar->setRange(0,recordedPackets.length());
    recordedSendTimer.start();
}



void MainWindow::menuTriggered() //действия меню
{
    QAction *action = (QAction*)sender();


    if (action == ui->saveAction){ //если нажали "сохранить"

       QString fileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить сценарий"), "C:", tr("RoboArm scenario files (*.ras)"));

       QFile file(fileName);

       sendTimer.stop();

        if(file.open(QIODevice::WriteOnly)){
            for (int i = 0; i <= recordedPackets.length()-1; i++){
             file.write(recordedPackets[i]);
            }
            file.close();
        }
        sendTimer.start();

    }


    if(action == ui->loadAction){ //если нажали "загрузить"

        recordedPackets.clear();

        QString filename = 0;

        filename = QFileDialog::getOpenFileName(this,
             tr("Загрузить сценарий"), "C:", tr("RoboArm scenario files (*.ras)"));
        QFile file(filename);
        QFileInfo fi(filename);

        QByteArray scenarioBuffer = 0;

        if(file.open(QIODevice::ReadOnly)){
               scenarioBuffer = file.readAll();
               sendTimer.stop();
                    for (int i = 5; i<=scenarioBuffer.length() ;i = i+5){
                         QByteArray secondBuffer = 0;
                         for(int a = i-5; a <=i+5; a++){
                         secondBuffer.append(scenarioBuffer[a]);
                     }
                    recordedPackets.append(secondBuffer);
                }
                 ui->filenameLable->setText("Сценарий " + fi.fileName());

        }

        file.close();

        sendTimer.start();
    }
}


void MainWindow::recordedSendTimerSlot() //воспроизведение сценария
{
   if (sendedPacketsCount <= recordedPackets.length()-1 )
   {
       if(serial.isOpen()){
           serial.write(recordedPackets[sendedPacketsCount]);
           sendedPacketsCount++;
           ui->progressBar->setValue(sendedPacketsCount);
       }
   }
   else
   {
       recordedSendTimer.stop();
       sendTimer.start();
   }
}


void MainWindow::on_connectButton_clicked()
{
    serial_init();

    if(serial_init()){
        sendTimer.start();
        ui->connectLabel->setText("Связь установлена");
    }
    else{
        ui->connectLabel->setText("Связь потеряна");
        qDebug() << "Error opening serial port, timer not started";
    }
}
