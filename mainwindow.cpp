#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QAction>
#include<QList>
#include<QMessageBox>
#include<QWidget>
#include"form.h"
#include<QByteArray>
#include<QByteArrayData>
#include<QTimer>
#include<QTimerEvent>
#include<QIODevice>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),TXD_Count(0),RXD_Count(0)
{
    ui->setupUi(this);
    ui->actionPause_P->setDisabled(true);
    ui->actionStop_S->setDisabled(true);
    ui->actionStart->setDisabled(true);
    ui->actionReceive_R->setDisabled(true);
    ui->TimeCheck->setDisabled(true);
    /*启动时扫面端口*/
    serial =new QSerialPort;
    foreach( const QSerialPortInfo &Info,QSerialPortInfo::availablePorts())//读取串口信息
    {
        qDebug() << "portName    :"  << Info.portName();
        qDebug() << "Description   :" << Info.description();
        qDebug() << "Manufacturer:" << Info.manufacturer();

        serial->setPort(Info);

        if( serial->open( QIODevice::ReadWrite) )//如果串口是可以读写方式打开的
        {
            ui->PortNumBox->addItem( Info.portName() );
            serial->close();//然后自动关闭等待人为开启
        }
        ui->actionStart->setEnabled(true);
        ui->TimeCheck->setEnabled(true);
    }
    ui->DataBitBox->setCurrentText("8");//定位到8，方便设置
    ui->R_ASSIC->setChecked(true);
    ui->S_ASSIC->setChecked(true);
    ui->dockWidget->setWindowTitle(tr("settingDock"));
    ui->dockWidget_5->setWindowTitle(tr("toolDock"));
    serial->close();
    //用于定时发送
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::SendData);
    ui->statusBar->showMessage("welcome use SerialPortAssiant!",4000);//显示欢迎
    /*收发数据统计 显示在状态栏*/
    QLabel *showTxd=new QLabel(this);
    showTxd->setText("TXD: ");
    showTxd->setFrameStyle(QFrame::Box|QFrame::Sunken);
    showTxd2=new QLabel(this);
    showTxd2->setNum(TXD_Count);
    ui->statusBar->addPermanentWidget(showTxd);
    ui->statusBar->addPermanentWidget(showTxd2);
    QLabel *showRxd=new QLabel(this);
    showRxd->setText("RXD: ");
    showRxd->setFrameStyle(QFrame::Box|QFrame::Sunken);
    showRxd2=new QLabel(this);
    showRxd2->setNum(RXD_Count);
    ui->statusBar->addPermanentWidget(showRxd);
    ui->statusBar->addPermanentWidget(showRxd2);
    ui->ShowDataNumLabel->setNum(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StringToHex(QString str, QByteArray & senddata)  //字符串转换成16进制数据0-F
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

char MainWindow::ConvertHexChar(char ch)//十六进制转char
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return ch-ch;//不在0-f范围内的会发送成0
}

void MainWindow::ReadSerialPortData()//串口接收函数
{
    requestData = serial->readAll();
    RXD_Count+=requestData.length();
    showRxd2->setNum(RXD_Count);//显示接收到的额数据总个数；
    if(!requestData.isEmpty())
    {
        if(ui->AutoLineFeed->isChecked()||ui->ShowTimeCheck->isChecked())
        {//显示时间也要换行
            if(ui->ShowTimeCheck->isChecked())
            {
                Time=QTime::currentTime();
                ui->ReceiveWin->appendPlainText(Time.toString("hh:mm:ss :"));
                ui->ReceiveWin->moveCursor(QTextCursor::End);
                ui->ReceiveWin->insertPlainText(requestData);
            }else{
                ui->ReceiveWin->appendPlainText(requestData);
            }
        }else{
            ui->ReceiveWin->moveCursor(QTextCursor::End);
            ui->ReceiveWin->insertPlainText(requestData);
        }
    }
    if(ui->CountCheck->isChecked()){
        if(requestData==ui->CountEdit->text()){
            Count++;
            ui->ShowDataNumLabel->setNum(Count);
            ui->ShowDataNumLabel->show();
            qDebug()<<"Count"<<Count;
        }
    }
    qDebug()<<"RECEIVEdata"<<requestData;
    requestData.clear();
}

void MainWindow::on_actionRefresh_R_triggered() //刷新端口扫描
{
    ui->PortNumBox->clear();
    ui->actionStart->setDisabled(true);
    ui->TimeCheck->setDisabled(true);
    foreach( const QSerialPortInfo &Info,QSerialPortInfo::availablePorts())//读取串口信息
        {
            qDebug() << "portName    :"  << Info.portName();//调试时可以看的串口信息
            qDebug() << "Description   :" << Info.description();
            qDebug() << "Manufacturer:" << Info.manufacturer();
            serial->setPort(Info);

            if( serial->open( QIODevice::ReadWrite) )//如果串口是可以读写方式打开的
            {
                ui->PortNumBox->addItem( Info.portName() );
                serial->close();
                //serial->clear();
            }
            ui->actionStart->setEnabled(true);
            ui->TimeCheck->setEnabled(true);
        }
}
//*端口控制槽函数*/
void MainWindow::on_actionStart_triggered()
{
    ui->actionStart->setDisabled(true);//互斥设置
    ui->actionRefresh_R->setDisabled(true);
    ui->PortNumBox->setDisabled(true);
    ui->actionStop_S->setEnabled(true);
    ui->actionReceive_R->setEnabled(true);
   serial->setPortName( ui->PortNumBox->currentText() );
   serial->close();
   serial->open( QIODevice::WriteOnly );//以只写方式打开
   qDebug() << ui->PortNumBox->currentText();
   serial->setBaudRate(  ui->BuatrateBox->currentText().toInt() );//波特率
   if(ui->DataBitBox->currentText().toInt()==5)
       serial->setDataBits( QSerialPort::Data5 );
   else if(ui->DataBitBox->currentText().toInt()==6)
       serial->setDataBits( QSerialPort::Data6 );
   else if(ui->DataBitBox->currentText().toInt()==7)
       serial->setDataBits( QSerialPort::Data7 );
   else if(ui->DataBitBox->currentText().toInt()==8)
       serial->setDataBits( QSerialPort::Data8 );

   if(ui->StopBitBox->currentText().toFloat()==1.0)//停止位
       serial->setStopBits(QSerialPort::OneStop);
    else if(ui->StopBitBox->currentText().toFloat()==1.5)
       serial->setStopBits(QSerialPort::OneAndHalfStop);
    else if(ui->StopBitBox->currentText().toFloat()==2.0)
       serial->setStopBits(QSerialPort::TwoStop);

   if(ui->CheckBitBox->currentText()=="None")  //校验位
       serial->setParity(QSerialPort::NoParity);
   else if(ui->CheckBitBox->currentText()=="Even")
       serial->setParity(QSerialPort::EvenParity);
   else if(ui->CheckBitBox->currentText()=="Odd")
       serial->setParity(QSerialPort::OddParity);
   else if(ui->CheckBitBox->currentText()=="Space")
       serial->setParity(QSerialPort::SpaceParity);

   if(ui->FlowCtrl->currentText()=="None")  //流控
        serial->setFlowControl( QSerialPort::NoFlowControl );
   else if(ui->FlowCtrl->currentText()=="RTS/CTS")
        serial->setFlowControl( QSerialPort::HardwareControl );
   else if(ui->FlowCtrl->currentText()=="XON/XOFF")
        serial->setFlowControl( QSerialPort::SoftwareControl );

}

void MainWindow::on_actionReceive_R_triggered()
{
    ui->actionReceive_R->setDisabled(true);
    ui->actionPause_P->setEnabled(true);
    ui->actionStop_S->setEnabled(true);
    serial->close();
    serial->open(QIODevice::ReadWrite);
    connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadSerialPortData);
    qDebug()<<"receive";

}

void MainWindow::on_actionPause_P_triggered()
{
    ui->actionStart->setEnabled(false); //互斥设置
    ui->actionPause_P->setDisabled(true);
    ui->actionReceive_R->setEnabled(true);
    ui->actionStop_S->setEnabled(true);
   // disconnect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadSerialPortData);
    serial->clear();
    serial->close();  //切换模式时一定要先关后开  否则不成功
    serial->open(QIODevice::WriteOnly);
    qDebug()<<"pause";
}

void MainWindow::on_actionStop_S_triggered()
{
    //ui->actionStop_S->setDisabled(true);//互斥设置
    ui->actionStart->setEnabled(true);
    ui->actionPause_P->setDisabled(true);
    ui->actionReceive_R->setDisabled(true);
    ui->PortNumBox->setEnabled(true);
    ui->actionRefresh_R->setEnabled(true);
    serial->close();
    qDebug()<<"close";
}

void MainWindow::on_actionClear_C_triggered()
{
    ui->ReceiveWin->clear();
}
/*串口设置相关槽函数*/
void MainWindow::on_BuatrateBox_currentTextChanged(const QString &arg1)
{
   // serial->close();
    serial->setBaudRate( arg1.toInt() );//波特率
    //serial->open(QIODevice::ReadWrite);
    qDebug()<<"buat"<<arg1;
}

void MainWindow::on_DataBitBox_currentTextChanged(const QString &arg1)
{
    //serial->close();
    if(arg1.toInt()==5)
        serial->setDataBits( QSerialPort::Data5 );
    else if(arg1.toInt()==6)
        serial->setDataBits( QSerialPort::Data6 );
    else if(arg1.toInt()==7)
        serial->setDataBits( QSerialPort::Data7 );
    else if(arg1.toInt()==8)
        serial->setDataBits( QSerialPort::Data8 );
    //serial->open(QIODevice::ReadWrite);
    qDebug()<<"data"<<serial->dataBits();
}

void MainWindow::on_StopBitBox_currentTextChanged(const QString &arg1)
{
    //serial->close();
    if(arg1.toFloat()==1.0)
        serial->setStopBits(QSerialPort::OneStop);
     else if(arg1.toFloat()==1.5)
        serial->setStopBits(QSerialPort::OneAndHalfStop);
     else if(arg1.toFloat()==2.0)
        serial->setStopBits(QSerialPort::TwoStop);
    //serial->open(QIODevice::ReadWrite);
    qDebug()<<"stop"<<serial->stopBits();

}

void MainWindow::on_CheckBitBox_currentTextChanged(const QString &arg1)
{
    // serial->close();
     if(arg1=="None")
         serial->setParity(QSerialPort::NoParity);
     else if(arg1=="Even")
         serial->setParity(QSerialPort::EvenParity);
     else if(arg1=="Odd")
         serial->setParity(QSerialPort::OddParity);
     else if(arg1=="Space")
         serial->setParity(QSerialPort::SpaceParity);
   //  serial->open(QIODevice::ReadWrite);
     qDebug()<<"check"<<serial->parity();
}

void MainWindow::on_FlowCtrl_currentTextChanged(const QString &arg1)
{
    //serial->close();
    if(arg1=="None")
         serial->setFlowControl( QSerialPort::NoFlowControl );
    else if(arg1=="RTS/CTS")
         serial->setFlowControl( QSerialPort::HardwareControl );
    else if(arg1=="XON/XOFF")
         serial->setFlowControl( QSerialPort::SoftwareControl );
   // serial->open(QIODevice::ReadWrite);
    qDebug()<<"check"<<serial->flowControl();
}
/*串口相关槽函数结尾*/
void MainWindow::on_Send_S_Button_clicked()
{
    if(ui->TimeCheck->isChecked()){
        if(ui->TimeEdit->text()!=""){
        timer->start(ui->TimeEdit->text().toInt());
        qDebug()<<"auto send "<<ui->TimeEdit->text().toInt();
        ui->Send_S_Button->setDisabled(true);
        ui->TimeEdit->setDisabled(true);
        ui->TimeCheck->setDisabled(true);
        }else{
            QMessageBox::warning(this,"no time input","please input autoSend time!");
        }
    }
    else{
        timer->stop();
        SendData();
    }

}

void MainWindow::on_Stop_S_Button_clicked()
{
    timer->stop();
    ui->Send_S_Button->setEnabled(true);
    ui->TimeEdit->setEnabled(true);
     ui->TimeCheck->setEnabled(true);
}

void MainWindow::SendData()//通用发送函数(公有)
{
    if(serial->isOpen())
    {
    if(ui->S_ASSIC->isChecked()){
        serial->write(ui->SendWindow->text().toUtf8());
        ui->HistoryWin->addItem(ui->SendWindow->text().toUtf8());
        TXD_Count+=ui->SendWindow->text().length();//发送计数
        qDebug()<<"string"<<ui->SendWindow->text().toUtf8();
    }
    else if(ui->S_HEX->isChecked()){
        QByteArray Dat=ui->SendWindow->text().toUtf8();
        qDebug()<<"Dat"<<Dat;
        QByteArray dat;
        for(int i=0;i<Dat.length();i++){
            dat.append(ConvertHexChar(Dat[i]));
        }
        qDebug()<<"string"<<dat;
        serial->write(dat);
        TXD_Count+=dat.length();//发送计数
        ui->HistoryWin->addItem(ui->SendWindow->text().toUtf8());
     }
    }else{
        QMessageBox::warning(this,tr("port not open"),tr("please open port"));
    }
    showTxd2->setNum(TXD_Count);//发送完成后刷新发送总个数
}

void MainWindow::on_ClearHistory_clicked()
{
    ui->HistoryWin->clear();
    qDebug()<<"clear win";
}

void MainWindow::on_HistoryWin_currentTextChanged(const QString &arg1)
{
    ui->SendWindow->setText(arg1.toUtf8());
}
/*设置工具箱控制相关槽函数*/
void MainWindow::on_actionOpen_Dock_triggered()
{
    if(ui->dockWidget->isHidden()){
        ui->dockWidget->setVisible(true);
    }
}

void MainWindow::on_actionClose_Dock_triggered()
{
    if(ui->dockWidget->isVisible()){
        ui->dockWidget->setHidden(true);
    }
}

void MainWindow::on_actionSetting_S_triggered()
{
    ui->dockWidget->setHidden(false);
    ui->dockWidget->setFloating(true);
}

void MainWindow::on_actionclose_triggered()
{
    close();
}

void MainWindow::on_actionDefault_Setting_triggered()
{
    serial->close();
    serial->setPortName(ui->PortNumBox->currentText());
    serial->open( QIODevice::ReadWrite );
    serial->setBaudRate( 9600 );//波特率
    serial->setDataBits( QSerialPort::Data8 );//数据字节，8字节
    serial->setParity( QSerialPort::NoParity );//校验，无
    serial->setFlowControl( QSerialPort::NoFlowControl );//数据流控制,无
    serial->setStopBits( QSerialPort::OneStop );//一位停止位
    /*更新设置*/
    ui->BuatrateBox->setCurrentText("9600");
    ui->DataBitBox->setCurrentText("1");
    ui->CheckBitBox->setCurrentText("None");
    ui->FlowCtrl->setCurrentText("None");
    ui->StopBitBox->setCurrentText("1");
}

void MainWindow::on_actionTouch_us_triggered()
{

    Form *form=new Form;
    form->setWindowTitle(tr("Touch us"));
    form->exec();
}


void MainWindow::on_ClearCountButton_clicked()
{
    Count=0;
    ui->ShowDataNumLabel->setNum(Count);
    ui->ShowDataNumLabel->show();

}

void MainWindow::on_CountEdit_textChanged(const QString &arg1)
{
    Count=0;
}

