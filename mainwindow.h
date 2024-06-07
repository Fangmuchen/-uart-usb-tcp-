/**
* This program code is written by Mr. fang for
* participating in the test and development
* interview of Zhuhai Jiabo Technology.
* The copyright belongs to the original author.
* It is only for reference and study.
* If you have any questions, please contact 2571905717@qq.com.
* Thank you for your cooperation and wish you a happy life.
*
*    @file:    mainwindow.h
*    @brief:
*    @author:  2571905717@qq.com
*    @date:    2024-06-07  11:36
**/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QCheckBox>
#include <QTextEdit>
#include <QTcpSocket>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void get_nowtime();
    void serialprot_init();
    void serialerror();
    void send_uart();

    void Tcp_disconnected();
    void Tcp_connected();
    void Tcp_sendtext();
    void Tcp_rev();
signals:


public:
    QString nowtime;
    QString tabWidget = "UART";
    QString ip ;
    quint16 prot;
    QString tcp_revtext;
    QString tcp_sendtext;


private slots:
    void on_pushButton_uartconnect_clicked();

    void on_pushButton_uartcut_clicked();

    void on_send_pushButton_clicked();

    void on_clear_pushButton_clicked();

    void rev_uartText();

    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void on_tabWidget_jiekou_currentChanged(int index);

    void on_clear_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *mserial;
    QTcpSocket *msocket;

};
#endif // MAINWINDOW_H
