#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QTime>
#include <QMainWindow>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QLabel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void StringToHex(QString str, QByteArray & senddata);
    char ConvertHexChar(char ch) ;
    void ReadSerialPortData();
    void SendData();
    ~MainWindow();
private slots:
    void on_actionRefresh_R_triggered();

    void on_actionStart_triggered();

    void on_actionPause_P_triggered();

    void on_actionStop_S_triggered();

    void on_BuatrateBox_currentTextChanged(const QString &arg1);

    void on_DataBitBox_currentTextChanged(const QString &arg1);

    void on_StopBitBox_currentTextChanged(const QString &arg1);

    void on_CheckBitBox_currentTextChanged(const QString &arg1);

    void on_FlowCtrl_currentTextChanged(const QString &arg1);

    void on_Send_S_Button_clicked();

    void on_ClearHistory_clicked();

    void on_HistoryWin_currentTextChanged(const QString &arg1);

    void on_actionOpen_Dock_triggered();

    void on_actionClose_Dock_triggered();

    void on_actionClear_C_triggered();

    void on_actionclose_triggered();

    void on_actionSetting_S_triggered();

    void on_actionDefault_Setting_triggered();

    void on_actionTouch_us_triggered();

    void on_actionReceive_R_triggered();

    void on_Stop_S_Button_clicked();

    void on_ClearCountButton_clicked();

    void on_CountEdit_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray requestData;//（用于存储从串口那读取的数据）
    QTimer *timer;//（用于计时）
    QTime Time;
    QLabel *showTxd2,*showRxd2;
    int TXD_Count,RXD_Count; //发送 接收计数
    int Count;//查询计数
};

#endif // MAINWINDOW_H
