#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtCharts>
#include<QLineSeries>
#include <QMainWindow>
#include <QtCharts/QChartGlobal>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QList>
#include<QtCharts/QLineSeries>
#include<QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class SettingsDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT  
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
   QString fileName ;
   QByteArray buffer;
   QByteArray ReceivedData;
   bool ClearGraph = false;
   char CaptureFlag = 0;
   int counter=0;
   int U32_gfSensorDatastarted=0U;
   int U32_gfSensorDatastoped=0U;
   QByteArray Sc8_gaSensor1Buffer,Sc8_gaSensor2Buffer,Sc8_gaSensor3Buffer;
   int Uc8_gvSensorCnt=0U;
   char Sc8_gvVar;

   struct ErrorMessage {
       QString message;
       int priority;
   };

   enum class State { ExpectingFirstString, ExpectingSecondString, ExpectingThirdString, Finished };
       State state = State::ExpectingFirstString;

private slots:

   void handleError(QSerialPort::SerialPortError error);
    void setDataGraph1(const QVector<double> &xData, const QVector<double> &yData1);
    void setDataGraph3(const QVector<double> &xData, const QVector<double> &yData2);
    void setDataGraph2(const QVector<double> &xData, const QVector<double> &yData3);
//    void on_checkBoxGraph1_stateChanged();
//    void on_checkBoxGraph2_stateChanged();
//    void on_checkBoxGraph3_stateChanged();
    void recenterGraph1();
    void updateXAxisRange(int xRange);
    void updateYAxisRange(int yRange);
    void updateRange();
    void openSerialPort();
    void closeSerialPort();
    void showStatusMessage(const QString &message);
    void on_CaptureData_clicked();
    void showSuccessMessage(const QString &message);
    void hideSuccessMessage();
    void openFile();
    void on_LoadfileButton_clicked();
    void on_ClearGraph_clicked();
    void toggleIndicatorState();
    void readData2();
    void readData();
    void plotData1(const QString &filePath);
    void on_pushButtonCenter1_clicked();

    void on_pushButtonCenter2_clicked();

    void on_pushButtonCenter3_clicked();

    void recenterGraph2();
    void recenterGraph3();
    void LastCapturedDataGraph();
    void saveFile();
    void onCheckBoxStateChanged();
private:


    QList<QLineSeries *> m_series;
    QList<QChartView *> m_charts;
    Ui::MainWindow *ui;
    QSerialPort *mSerial;
    QList<QSerialPortInfo> mSerialPorts;
    QTimer *mSerialScanTimer;
    SettingsDialog *m_settings = nullptr;
    QLabel *m_status = nullptr;
    QTimer *messageTimer;
    QMessageBox *popupMessage;
    QTimer *timer1;
    QTimer *delayTimer;

};
#endif // MAINWINDOW_H
