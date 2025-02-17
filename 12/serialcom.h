#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QWidget>
#include <QObject>
/*
 *<QSerialPort>:提供串口相关的服务
 *<QSerialPortInfo>:提供串口设备查询的服务
 */
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <functional>

#include <QTimer>
#include <QTime>
#include <QQueue>

/**
在MainWindow新建一个Widget组件，将其提升为SerialCom，假如这个组件命名为 widget_serial_port

发送函数：ui->widget_serial_port->writedata(ui->lineEdit->text().toLatin1());  入参类型为QByteArray

接收函数：比如就在MainWindow类中接收，增加槽函数
mainwindow.h
private slots:
    void serial_receive_data(QByteArray data);

mainwindow.cpp
void MainWindow:: serial_receive_data(QByteArray data)
{
    qDebug() << "receive:"<<data;
}
并且在MainWindow实例化中进行信号连接
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->widget_serial_port, &SerialCom::readdata, this, &MainWindow::serial_receive_data);
}
*/

namespace Ui {
class SerialCom;
}

class SerialCom : public QWidget
{
    Q_OBJECT

public:
    explicit SerialCom(QWidget *parent = nullptr);
    ~SerialCom();

    bool isopen_status;
    void writedata(QByteArray data);

signals:
    void readdata(const QByteArray& data);
    void StatusChange(bool status);

private slots:
    void receivedata();
    void SendData_Regularly();

    void on_btn_scancom_clicked();

    void on_btn_togglecom_clicked();

private:
    Ui::SerialCom *ui;

    QSerialPort  *serialport;
    QTimer       *send_data_timer;
    QQueue<QByteArray> *send_data_queue;

    void Open(void);
    void Close(void);

};

#endif // SERIALCOM_H
