#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QTextCodec>
#include <QByteArray>
#include <QDateTime>
#include <QMessageBox>
#include <QHostAddress>
#include <QTextCodec>
#include <QTextEdit>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->log_Edit->setWordWrapMode(QTextOption::WordWrap);
    //初始化socket
    msocket = new QTcpSocket(this);

    connect(msocket,&QTcpSocket::readyRead,this,&MainWindow::Tcp_rev);
    connect(msocket,&QTcpSocket::connected,this,&MainWindow::Tcp_connected);
    connect(msocket,&QTcpSocket::disconnected,this,&MainWindow::Tcp_disconnected);

    serialprot_init(); //初始化serial
    ui->send_Edit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->send_Edit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);//自动换行
    ui->send_Edit->setWordWrapMode(QTextOption::WrapAnywhere);
    connect(mserial,&QSerialPort::readyRead,this,&MainWindow::rev_uartText);



}

MainWindow::~MainWindow()
{
    delete ui;
    delete msocket;
}



void MainWindow::get_nowtime()
{
    //获取当前时间函数
    QDateTime timer = QDateTime::currentDateTime();
    QTextStream s_time;
    nowtime = timer.toString("hh:mm:ss:");
}

void MainWindow::serialprot_init()
{
    //获取串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBox_dk->addItem(info.portName());
    }
    //初始化serial
    mserial = new QSerialPort(this);
    mserial->setPortName(ui->comboBox_dk->currentText());
}

void MainWindow::serialerror()
{

    switch (mserial->error()) {
     case QSerialPort::NoError:
         break;
     case QSerialPort::DeviceNotFoundError:
         qDebug() << "串口设备未找到";
         break;
     case QSerialPort::PermissionError:
         qDebug() << "没有权限访问串口";
         break;
     case QSerialPort::OpenError:
         qDebug() << "串口打开失败";
         break;
    default:break;
    }
}

void MainWindow::send_uart()
{
    get_nowtime();
    QString send_Text = ui->send_Edit->toPlainText();
    //QByteArray send_Date =send_Text.toLocal8Bit();
    QByteArray utf8Text = send_Text.toUtf8(); // 首先转换为 UTF-8 编码
    // 获取 GB18030 编码的 QTextCodec 对象
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    // 使用 QTextCodec 转换为 GB18030 编码
    QByteArray send = codec->fromUnicode(send_Text);
    QByteArray send_Date=send;
    bool is_checked = ui->checkBox_log->isChecked();
    if(is_checked){
        QByteArray send_Date = send.toHex();
    }

    qint64 send_bytes= send_Date.size();
    QString bytes = QString::number(send_bytes);
    serialerror();
    if(mserial->isOpen()){
        mserial->write(send_Date);
        QCoreApplication::processEvents();

        qDebug() << "串口数据发送成功，发送字节数:"<<bytes;
        ui->sendlog->append(nowtime+"串口发送成功，"+bytes+"字节");
     }
    else {
        QMessageBox::critical(0,"错误","串口未打开",QMessageBox::Ok);
        ui->sendlog->append(nowtime+"串口发送失败，"+bytes+"字节");
        qDebug() << "串口数据发送失败:"<<send_bytes;
    }
}



void MainWindow::on_pushButton_uartconnect_clicked()
{
    get_nowtime();
    //端口
    mserial->setPortName(ui->comboBox_dk->currentText());
    //波特率
    mserial->setBaudRate(ui->comboBox_bt->currentText().toInt());
    qDebug()<<ui->comboBox_bt->currentText().toInt();
    //停止位
    switch (ui->comboBox_tz->currentText().toInt()) {
    case 1:mserial->setStopBits(QSerialPort::OneStop);break;
    case 2:mserial->setStopBits(QSerialPort::TwoStop);break;
        default:break;
    }
    //数据位
    switch (ui->comboBox_sj->currentText().toInt()) {
    case 1:mserial->setDataBits(QSerialPort::Data5);break;
    case 2:mserial->setDataBits(QSerialPort::Data6);break;
    case 3:mserial->setDataBits(QSerialPort::Data7);break;
    case 4:mserial->setDataBits(QSerialPort::Data8);break;
        default:break;
    }
    //校验位
    switch (ui->comboBox_jy->currentText().toInt()) {
    case 1:mserial->setParity(QSerialPort::NoParity);;break;
    case 2:mserial->setParity(QSerialPort::OddParity);break;
    case 3:mserial->setParity(QSerialPort::EvenParity);break;
        default:break;
    }
    //控制位
    switch (ui->comboBox_kz->currentText().toInt()) {
    case 1:mserial->setFlowControl(QSerialPort::NoFlowControl);break;
     case 2:mserial->setFlowControl(QSerialPort::HardwareControl);break;
    case 3:mserial->setFlowControl(QSerialPort::SoftwareControl);break;

        default:break;
    }
    if(mserial->open(QIODevice::ReadWrite))
    {
        serialerror();
        if(mserial->isOpen()){
        ui->log_Edit->append(nowtime+"串口打开成功,，暂未接收到数据！");
        ui->pushButton_uartconnect->setEnabled(false);
        }
    }
    else {
        ui->log_Edit->append(nowtime+"串口打开失败！");
    }
}

void MainWindow::on_pushButton_uartcut_clicked()
{
    //关闭串口
    serialprot_init();
    mserial->close();
    if(mserial->isOpen()){
        ui->sendlog->append(nowtime+"串口关闭失败！");

    }
    else{
        ui->log_Edit->append(nowtime+"串口已关闭！！！");
        ui->sendlog->append(nowtime+"串口关闭成功！");
        ui->pushButton_uartconnect->setEnabled(true);
    }
}

void MainWindow::on_send_pushButton_clicked()
{
    QString tab_uart = "UART";
    QString tab_usb = "USB";
    QString tab_tcp = "TCP";
    qDebug()<<tabWidget;
    if(bool Tab_select=(tab_uart==tabWidget)) {
    send_uart();}
    if(bool Tab_select=(tab_usb==tabWidget)) {


    }else if(bool Tab_select=(tab_tcp==tabWidget)){
        if(msocket->state()==QTcpSocket::ConnectedState){
            Tcp_sendtext();
        }else{
        QMessageBox::critical(0,"错误","TcpSocket未连接",QMessageBox::Ok);
        }
    }




}

void MainWindow::on_clear_pushButton_clicked()
{
    ui->send_Edit->clear();
}

void MainWindow::rev_uartText()
{
    QByteArray rev_data = mserial->readAll();
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    QString rev_text = codec->toUnicode(rev_data);
    if(ui->checkBox_log->isChecked())
    {
        ui->log_Edit->append(nowtime+"成功切换16进制");
        QString rev_dataText = rev_data.toHex();        //16进制显示
        ui->log_Edit->append(nowtime+rev_dataText);
    }else{
        QString rev_dataText = rev_text; //中文显示
        ui->log_Edit->append(nowtime+rev_dataText);
    }


}

void MainWindow::on_pushButton_connect_clicked()
{
    ip = ui->lineEdit_server_2->text();
    prot = ui->lineEdit_prot_2->text().toInt();
    qDebug()<<"tcp连接开始";
    qDebug()<<ip<<prot;
    msocket->open(QIODevice::ReadWrite);
    if(msocket->isOpen())
    {
        msocket->connectToHost(ip,prot);
    }
}

void MainWindow::on_pushButton_disconnect_clicked()
{
     msocket->close();
}



void MainWindow::Tcp_sendtext()
{
    get_nowtime();

    tcp_sendtext = ui->send_Edit->toPlainText();
    QByteArray tcp_sendbyte=tcp_sendtext.toLocal8Bit();
    if(msocket->isOpen()){
    msocket->write(tcp_sendbyte);
    qint64 send_tcpbyte= tcp_sendbyte.size();
    QString bytes = QString::number(send_tcpbyte);
    ui->sendlog->append(nowtime+"Tcp发送成功，"+bytes);
    }else{
        ui->sendlog->append(nowtime+"Tcp发送失败！！！");
    }
}

void MainWindow::Tcp_rev()
{   get_nowtime();
    QByteArray tcp_revbyte = msocket->readAll();
    if(ui->checkBox_log->isChecked()){
    ui->sendlog->append(nowtime+"成功切换16进制！");
    QString tcp_revtext = tcp_revbyte.toHex();
    }else{
    QString tcp_revtext = QString::fromLocal8Bit(tcp_revbyte);
    }
    ui->log_Edit->append(nowtime+"tcp连接成功，收到数据！");
    ui->log_Edit->append(nowtime+tcp_revtext);
}
void MainWindow::Tcp_connected(){
    get_nowtime();
    ui->log_Edit->append(nowtime+"Tcp连接成功！！！");
}

void MainWindow::Tcp_disconnected(){
    get_nowtime();
    ui->log_Edit->append(nowtime+"Tcp已断开！");
    if(msocket->error() == QTcpSocket::ConnectionRefusedError){
        QMessageBox::critical(0,"错误","服务器拒绝接入，是否重连？",QMessageBox::Ok);
        ui->log_Edit->append(nowtime+"Tcp断开，正在重新连接！");
        msocket->open(QIODevice::ReadWrite);
        for (int i= 0;i<99999;i++) {
            ui->log_Edit->append(nowtime+"正在重连中"+i);
            if(msocket->state()==QTcpSocket::ConnectedState){
                ui->log_Edit->append(nowtime+"重连成功");
                break;
            }
        }
        ui->log_Edit->append(nowtime+"重连超时！");
     }
}

void MainWindow::on_clear_pushButton_2_clicked()
{
    ui->log_Edit->clear();

}


void MainWindow::on_tabWidget_jiekou_currentChanged(int index)
{
\
    // 获取当前选中的选项卡的文本
    QString currentTabText = ui->tabWidget_jiekou->tabText(index);

    qDebug() << "当前选中的选项卡文本:" << currentTabText;

    tabWidget=currentTabText;
    qDebug()<<tabWidget;
}


