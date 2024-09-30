#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QtCharts/QLineSeries>
#include<QChartView>
#include<QValueAxis>
#include "settingsdialog.h"
#include <QFileDialog>
#include <QTimer>
#include<QFileInfo>
#include<QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_settings(new SettingsDialog),
    m_status(new QLabel)

{
    ui->setupUi(this);    

    QPixmap bkgnd(":/images/imagesblue.png");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
        QPalette palette;
        palette.setBrush(QPalette::Window, bkgnd);
        this->setPalette(palette);


    messageTimer = new QTimer(this);
    timer1 = new QTimer(this);
    delayTimer = new QTimer(this);
    mSerial = new QSerialPort(this);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setDisabled(true);
    ui->actionPort_Settings->setEnabled(true);
    ui->statusBar->addWidget(m_status,Qt::AlignCenter);
    ui->customplot1->xAxis->setLabel("X Axis  Sensor 1");
    ui->customplot1->yAxis->setLabel("Y Axis");
    ui->customplot2->xAxis->setLabel("X Axis  Sensor 2");
    ui->customplot2->yAxis->setLabel("Y Axis");
    ui->customplot3->xAxis->setLabel("X Axis  Sensor 3");
    ui->customplot3->yAxis->setLabel("Y Axis");
    ui->toolBar->setFixedHeight(35);
    ui->statusBar->setFixedHeight(30);
    ui->customplot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->customplot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->customplot3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    updateRange();
    connect(ui->checkBoxGraph1, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged()));
    connect(ui->checkBoxGraph2, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged()));
    connect(ui->checkBoxGraph3, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged()));

    connect(ui->comboBoxXAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(updateXAxisRange(int)));
    connect(ui->comboBoxYAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYAxisRange(int)));
    connect(mSerial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(mSerial, &QSerialPort::readyRead, this, &MainWindow::readData2);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPort_Settings, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(messageTimer, &QTimer::timeout, this, &MainWindow::hideSuccessMessage);
            popupMessage = new QMessageBox(this);
            popupMessage->setIcon(QMessageBox::Information);
    connect(timer1, &QTimer::timeout, this, &MainWindow::toggleIndicatorState);
    connect(delayTimer, &QTimer::timeout, this, &MainWindow::readData);



    ui->RedIndicator->setFixedSize(30, 30);
    ui->RedIndicator->setStyleSheet(" border-radius: 20px;");
    ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
    ui->lineEdit_FileName->setFixedWidth(300);
    ui->lineEdit_FileName->setFixedHeight(35);
    ui->GreenIndicator->setFixedSize(30, 30);
    ui->GreenIndicator->setStyleSheet("border-radius: 20px;");
    ui->GreenIndicator->setPixmap(QPixmap(":/images/greenoff.png"));

 }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    mSerial->setPortName(p.name);
    mSerial->setBaudRate(p.baudRate);
    mSerial->setDataBits(p.dataBits);
    mSerial->setParity(p.parity);
    mSerial->setStopBits(p.stopBits);
    mSerial->setFlowControl(p.flowControl);
    if (mSerial->open(QIODevice::ReadWrite)) {

        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionPort_Settings->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        ui->actionConnect->setToolTip(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
              qDebug()<<"connected";


    } else {
        showStatusMessage(tr("Open error"));
        QMessageBox::critical(this, tr("Error"), mSerial->errorString());

    }
}
void MainWindow::closeSerialPort()
{
    if (mSerial->isOpen())
        mSerial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionPort_Settings->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
    qDebug()<<"Disconnected";
    ui->GreenIndicator->setPixmap(QPixmap(":/images/greenoff.png"));
    ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
    timer1->stop();

    Sc8_gaSensor1Buffer.clear();
    Sc8_gaSensor2Buffer.clear();
    Sc8_gaSensor3Buffer.clear();

}
void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
    m_status->setAlignment(Qt::AlignCenter);

    m_status->setStyleSheet("QLabel {font: 600 12pt Segoe UI Semibold }");

}

void MainWindow:: readData()
{
    ui->GreenIndicator->setPixmap(QPixmap(":/images/greenon.png"));
    ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
    timer1->start(300);
     QByteArray U32_laSerialData = mSerial->readLine();
             buffer.append(U32_laSerialData);

             while(buffer.contains('\r')){

//                 QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

                 QByteArray U32_laSerialDataLine = buffer.mid(0, buffer.indexOf('\r') + 1);
                 buffer.remove(0, U32_laSerialDataLine.size());
                 QString lineString = QString::fromUtf8(U32_laSerialDataLine).trimmed();
                 QByteArray ReceivedArray = lineString.toUtf8();              

                 qDebug()<<ReceivedArray.size();


                 if(ReceivedArray.startsWith("$") && ReceivedArray.endsWith("#"))  {

                     switch (Uc8_gvSensorCnt) {
                                     case 0:
                                         Sc8_gaSensor1Buffer= ReceivedArray;
                                         qDebug()<<"Sensor1 data received";
                                         Uc8_gvSensorCnt=1U;
                                         break;
                                     case 1:
                                         Sc8_gaSensor2Buffer= ReceivedArray;
                                         qDebug()<<" Sensor2 data is received";
                                         Uc8_gvSensorCnt=2U;
                                         break;
                                     case 2:
                                         Sc8_gaSensor3Buffer= ReceivedArray;
                                         qDebug()<<"Sensor3 data is received";
                                         Uc8_gvSensorCnt=0U;
                                         delayTimer->start(2000);
                                         break;
                                     }

                                         qDebug()<<Uc8_gvSensorCnt;

                             }
                               else{
                                     qDebug()<<"error while receiving initial character";
                                   }

         }

}
void MainWindow:: readData2()
{
    ui->GreenIndicator->setPixmap(QPixmap(":/images/greenon.png"));
    ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
    timer1->start(500);
     QByteArray U32_laSerialData = mSerial->readAll();
             buffer.append(U32_laSerialData);

             while(buffer.contains('\r')){

                 QByteArray U32_laSerialDataLine = buffer.mid(0, buffer.indexOf('\r') + 1);
                 buffer.remove(0, U32_laSerialDataLine.size());
                 QString lineString = QString::fromUtf8(U32_laSerialDataLine).trimmed();
                 QByteArray ReceivedArray = lineString.toUtf8();

                 qDebug()<<"array size :"<<ReceivedArray.size();

                 if(state == State::ExpectingFirstString && ReceivedArray.contains("$")&& ReceivedArray.endsWith("#"))
                 {

                            Sc8_gaSensor1Buffer= ReceivedArray;
                            qDebug() << "Received first string: Sensor 1 ";
                            showStatusMessage(" Sensor 1 data Received");
                            showSuccessMessage("Sensor 1 data Received");

                            state = State::ExpectingSecondString;
                        }
                 else if (state == State::ExpectingSecondString && ReceivedArray.contains("$") && ReceivedArray.endsWith("#"))
                 {

                            Sc8_gaSensor2Buffer= ReceivedArray;
                            qDebug() << "Received second string: Sensor 2 ";
                            showStatusMessage(" Sensor 2 data Received");
                            showSuccessMessage("Sensor 2 data Received");


                            state = State::ExpectingThirdString;
                        }
                 else if (state == State::ExpectingThirdString && ReceivedArray.contains("$") &&  ReceivedArray.endsWith("#"))
                 {

                             Sc8_gaSensor3Buffer= ReceivedArray;
                            qDebug() << "Received third string: Sensor 3 ";
                            showStatusMessage(" Sensor 3 data Received");
                            showSuccessMessage("Sensor 3 data Received");


//                            state = State::Finished;
                            state= State::ExpectingFirstString;

                        } else
                 {
                            qDebug()<<"Error while receiving string ";
                            QMessageBox::critical(this, tr("Critical Error"),"unable to receive sensor data");

                 }

         }

}

void MainWindow::setDataGraph1(const QVector<double> &xData, const QVector<double> &yData1)
{    
    ui->customplot1->addGraph();
    ui->customplot1->xAxis->ticker()->setTickCount(10);
    ui->customplot1->graph(0)->setPen(QPen(QColor(0, 0, 112)));
    ui->customplot1->graph(0)->setData(xData, yData1);
    ui->customplot1->rescaleAxes();
    ui->customplot1->replot();

}
void MainWindow::setDataGraph2(const QVector<double> &xData, const QVector<double> &yData2)
{

    ui->customplot2->addGraph();
    ui->customplot2->xAxis->ticker()->setTickCount(10);
    ui->customplot2->graph(0)->setPen(QPen(QColor(218, 0, 0)));
    ui->customplot2->graph(0)->setData(xData, yData2);
    ui->customplot2->rescaleAxes();
    ui->customplot2->replot();


}
void MainWindow::setDataGraph3(const QVector<double> &xData, const QVector<double> &yData3)
{
    ui->customplot3->addGraph();
    ui->customplot3->xAxis->ticker()->setTickCount(10);
    ui->customplot3->graph(0)->setPen(QPen(Qt::darkGreen));
    ui->customplot3->graph(0)->setData(xData, yData3);
    ui->customplot3->rescaleAxes();
    ui->customplot3->replot();
}

void MainWindow::openFile()
{
    timer1->stop();
    QString filePath = QFileDialog::getOpenFileName(this, "Select a Text File", QDir::homePath(), "Text Files (*.txt)");

    if (!filePath.isEmpty()) {
        qDebug()<<fileName;
        ui->lineEdit_FileName->setText(filePath);

        plotData1(filePath);
    }
}
void MainWindow::plotData1(const QString &filePath)
{
    ClearGraph = true;
    ui->customplot1->clearGraphs();
    ui->customplot2->clearGraphs();
    ui->customplot3->clearGraphs();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file: " << filePath;
        return;
    }
    int lineCounter = 0;
    QStringList data1,data2,data3;
    QVector<double> doubleList;
    QVector<double> x, y1,y2,y3;
     double index =0;
     double indexlist;
     QString  currentLine;
    QTextStream in(&file);
    while (!in.atEnd())
    {
               QString line = in.readLine();

               if (line.startsWith('$') && line.endsWith('#'))
               {
                   currentLine = line.mid(1, line.length() - 2);

                   if (lineCounter == 0)
                   {
                       data1.append(currentLine);
                       data1 = currentLine.split(",");

                       foreach (const QString &str, data1)
                       {
                           bool ok;
                           double value = str.toDouble(&ok);
                           indexlist=index++;
                           if (ok) {
                               doubleList.append(value);
                               y1.append(value);
                               x.append(indexlist);
                           }

                           else {
                               qDebug() << "Conversion to double failed for:"<<str;
                           }
                       }
                   }
                   else if (lineCounter == 1)
                   {
                       data2.append(currentLine);
                       data2 = currentLine.split(",");

                       foreach (const QString &str, data2)
                       {
                           bool ok;
                           double value = str.toDouble(&ok);
                           indexlist=index++;
                           if (ok) {
                               doubleList.append(value);
                               y2.append(value);
                               x.append(indexlist);
                           } else {
                               qDebug() << "Conversion to double failed for:"<<str;
                           }
                       }
                   }
                   else if (lineCounter == 2) {
                       data3.append(currentLine);
                       data3 = currentLine.split(",");


                       foreach (const QString &str, data3) {
                           bool ok;
                           double value = str.toDouble(&ok);
                           indexlist=index++;
                           if (ok) {
                               doubleList.append(value);
                               y3.append(value);
                               x.append(indexlist);
                           } else {
                               qDebug() << "Conversion to double failed for:"<<str;
                           }
                       }
                   }

                   // Increment the line counter
                   lineCounter++;

                   // If we've read three lines, reset the counter
                   if (lineCounter == 3) {
                       lineCounter = 0;
                   }
               }
           }
           file.close();


           setDataGraph1(x,y1);
           setDataGraph2(x,y2);
           setDataGraph3(x,y3);
}

void MainWindow::recenterGraph1() {

       ui->customplot1->rescaleAxes();
       ui->customplot1->replot();

}
void MainWindow::recenterGraph2() {

    ui->customplot2->rescaleAxes();
    ui->customplot2->replot();
}
void MainWindow::recenterGraph3() {

    ui->customplot3->rescaleAxes();
    ui->customplot3->replot();
}

void MainWindow:: updateRange(){

    ui->comboBoxYAxis->addItem(" 300");
    ui->comboBoxYAxis->addItem(" 250");
    ui->comboBoxYAxis->addItem(" 100");
    ui->comboBoxYAxis->addItem(" 200");
    ui->comboBoxYAxis->addItem(" 400");
    ui->comboBoxYAxis->addItem(" 500");
    ui->comboBoxYAxis->addItem(" 50");


    ui->comboBoxXAxis->addItem("120000");
    ui->comboBoxXAxis->addItem("150000");
    ui->comboBoxXAxis->addItem("200000");
    ui->comboBoxXAxis->addItem("300000");
    ui->comboBoxXAxis->addItem("400000");
    ui->comboBoxXAxis->addItem("450000");
    ui->comboBoxXAxis->addItem("500000");
    ui->comboBoxXAxis->addItem("550000");
    ui->comboBoxXAxis->addItem("50");

}
void MainWindow::updateXAxisRange(int xRange) {
     xRange = ui->comboBoxXAxis->currentText().toInt();

    ui->customplot1->xAxis->setRange(0, xRange);
    ui->customplot1->replot();
    ui->customplot2->xAxis->setRange(0, xRange);
    ui->customplot2->replot();
    ui->customplot3->xAxis->setRange(0, xRange);
    ui->customplot3->replot();
}

void MainWindow::updateYAxisRange(int yRange) {
     yRange = ui->comboBoxYAxis->currentText().toInt();

    ui->customplot1->yAxis->setRange(0, yRange);
    ui->customplot1->replot();
    ui->customplot2->yAxis->setRange(0, yRange);
    ui->customplot2->replot();
    ui->customplot3->yAxis->setRange(0, yRange);
    ui->customplot3->replot();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), mSerial->errorString());
        ui->GreenIndicator->setPixmap(QPixmap(":/images/greenoff.png"));
        ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
        closeSerialPort();
    }

}
void MainWindow::saveFile() {
        // Open a file dialog to get the save location from the user
        QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Text Files (*.txt);;All Files (*)");

        // Check if the user selected a file
        if (!filePath.isEmpty()) {


     QFile file(filePath);
     if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream out(&file);
                if(!Sc8_gaSensor3Buffer.isEmpty()){

                  out<<Sc8_gaSensor1Buffer<<'\n'<<Sc8_gaSensor2Buffer<<'\n'<<Sc8_gaSensor3Buffer;

                  file.close();
                  Sc8_gaSensor1Buffer.clear();
                  Sc8_gaSensor2Buffer.clear();
                  Sc8_gaSensor3Buffer.clear();
                  showSuccessMessage(" data is Saved sucessfully ");
                  plotData1(filePath);
                }

            else {

                  if(!mSerial->isOpen()){

                      file.remove();
                      QMessageBox::critical(this, "Error", "Serial port is not connected.");
                      showSuccessMessage(" file not created ");

                     }

                  else{
                      file.remove();
                       showSuccessMessage(" Data is in progress or empty ");
                  }
                }
      }

        }
    }
void MainWindow::on_CaptureData_clicked()
{
   saveFile();
}

void MainWindow:: showSuccessMessage(const QString &message)
    {
    popupMessage->setText(message);
    popupMessage->show();
    messageTimer->start(2000);
    }
void MainWindow:: hideSuccessMessage()
    {
        popupMessage->hide();
        messageTimer->stop();
    }

void MainWindow::on_LoadfileButton_clicked()
{
    timer1->stop();
    if(!ClearGraph){
        openFile();     
    }
    else{
         QMessageBox::information(this, "Error", " Please clear recent graphs to select file.");
    }
}

void MainWindow::on_ClearGraph_clicked()
{
    ClearGraph = false;
    ui->customplot1->clearPlottables();
    ui->customplot2->clearPlottables();
    ui->customplot3->clearPlottables();
    ui->lineEdit_FileName->clear();
    ui->customplot1->clearGraphs();
    ui->customplot2->clearGraphs();
    ui->customplot3->clearGraphs();
    ui->customplot1->replot();
    ui->customplot2->replot();
    ui->customplot3->replot();
}
void MainWindow::toggleIndicatorState() {
     static bool isOn = false;
    if (isOn) {
          ui->RedIndicator->setPixmap(QPixmap(":/images/redoff.png"));
    } else {
        ui->RedIndicator->setPixmap(QPixmap(":/images/redon.png"));
    }
    isOn=!isOn;
}

void MainWindow::on_pushButtonCenter1_clicked()
{
   recenterGraph1();
}

void MainWindow::on_pushButtonCenter2_clicked()
{
  recenterGraph2();
}

void MainWindow::on_pushButtonCenter3_clicked()
{
 recenterGraph3();
}
void MainWindow::LastCapturedDataGraph()
{
    QString buildDir = QCoreApplication::applicationDirPath();
    QString folderName = "Captured_data";

    QDir folderDir(buildDir + QDir::separator() + folderName);
    QStringList nameFilters;
    nameFilters<<"DAQ_*.txt";  // Filter by file extension, adjust as needed
    folderDir.setNameFilters(nameFilters);
    QFileInfoList fileList = folderDir.entryInfoList(QDir::Files, QDir::Time);

    if (!fileList.isEmpty()) {
        QString mostRecentFilePath = fileList.first().absoluteFilePath();

        plotData1(mostRecentFilePath);
        ui->lineEdit_FileName->setText(mostRecentFilePath);
    }
    else
    {
        qDebug()<<"file list is empty";
//        ui->checkBoxGraph1->setVisible(false);
//        ui->checkBoxGraph2->setVisible(false);
//        ui->checkBoxGraph3->setVisible(false);
        showStatusMessage(tr(" File folder is empty"));

    }


}
void MainWindow::onCheckBoxStateChanged() {
    QCheckBox *senderCheckBox = qobject_cast<QCheckBox*>(sender());

    if (!senderCheckBox)
        return;

    // Keep track of selected checkboxes
    QList<QCheckBox *> selectedCheckboxes;
    if (ui->checkBoxGraph1->isChecked())
        selectedCheckboxes.append(ui->checkBoxGraph1);
    if (ui->checkBoxGraph2->isChecked())
        selectedCheckboxes.append(ui->checkBoxGraph2);
    if (ui->checkBoxGraph3->isChecked())
        selectedCheckboxes.append(ui->checkBoxGraph3);

    // Show or hide corresponding custom plot widgets
    ui->customplot1->setVisible(selectedCheckboxes.contains(ui->checkBoxGraph1));
    ui->customplot2->setVisible(selectedCheckboxes.contains(ui->checkBoxGraph2));
    ui->customplot3->setVisible(selectedCheckboxes.contains(ui->checkBoxGraph3));
}
