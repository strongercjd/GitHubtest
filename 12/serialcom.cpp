#include "serialcom.h"
#include "ui_serialcom.h"

SerialCom::SerialCom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialCom)
{
    ui->setupUi(this);

    serialport = nullptr;
    isopen_status = false;

    send_data_timer = new QTimer();
    send_data_timer->start(20);
    connect(send_data_timer,SIGNAL(timeout()),this,SLOT(SendData_Regularly()));
    send_data_queue = new QQueue<QByteArray>;

    /*扫描串口*/
    QStringList serialNamePort;
    int nn = this->ui->cb_com->count();
    for (int i =nn-1;i>-1; i--)
    {
        if (this->ui->cb_com->count() < 1) return;
        //if (isel < cb_com->currentIndex()) return;
        this->ui->cb_com->removeItem(i);
    }
    /* 搜索所有可用串口 */
    foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()) {
        serialNamePort<<inf0.portName();
    }
    this->ui->cb_com->addItems(serialNamePort);


    /* 增加波特率 */
    ui->cb_bsp->clear();
    ui->cb_bsp->addItem ("1200");
    ui->cb_bsp->addItem ("2400");
    ui->cb_bsp->addItem ("4800");
    ui->cb_bsp->addItem ("9600");
    ui->cb_bsp->addItem ("19200");
    ui->cb_bsp->addItem ("38400");
    ui->cb_bsp->addItem ("57600");
    ui->cb_bsp->addItem ("115200");

    /* 默认选择115200波特率 */
    ui->cb_bsp->setCurrentIndex (7);
}

SerialCom::~SerialCom()
{
    Close();

    disconnect(send_data_timer,SIGNAL(timeout()),this,SLOT(SendData_Regularly()));
    delete send_data_timer;

    delete send_data_queue;
    delete ui;
}
/**
 * @brief SerialCom::扫描串口
 */
void SerialCom::on_btn_scancom_clicked()
{
    /*扫描串口*/
    QStringList serialNamePort;
    int nn = this->ui->cb_com->count();
    for (int i =nn-1;i>-1; i--)
    {
        if (this->ui->cb_com->count() < 1) return;
        this->ui->cb_com->removeItem(i);
    }
    /* 搜索所有可用串口 */
    foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()) {
        serialNamePort<<inf0.portName();
    }
    this->ui->cb_com->addItems(serialNamePort);
}
/**
 * @brief SerialCom::打开或关闭串口
 */
void SerialCom::on_btn_togglecom_clicked()
{
    if(isopen_status)//串口正在打开
    {
        Close();
    }else{
        Open();
    }
    emit StatusChange(isopen_status);
}

void SerialCom::Open(void)
{
    //打开串口
    if(serialport != nullptr)
    {
        delete serialport;
    }
    serialport = new QSerialPort();
    if(serialport->isOpen() == false)
    {
        /* 串口设置 */
        serialport->setPortName(ui->cb_com->currentText());
        serialport->setBaudRate(ui->cb_bsp->currentText().toInt());
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setStopBits(QSerialPort::OneStop);
        serialport->setParity(QSerialPort::NoParity);
        serialport->open(QIODevice::ReadWrite);
        if(serialport->isOpen())
        {
            isopen_status = true;
            connect(serialport,SIGNAL(readyRead()),this,SLOT(receivedata()));
            ui->btn_togglecom->setText("关闭串口");
            qDebug()<<"SerialOpen is ok";
        }
        else
        {
            isopen_status = false;
            qDebug()<<"SerialOpen is fail";
        }
    }
    else
    {
        serialport->close();
        isopen_status = false;
        qDebug()<<"SerialOpen is error";
    }
}

void SerialCom::Close(void)
{
    ui->btn_togglecom->setText("打开串口");
    if(isopen_status)//串口正在打开
    {
        serialport->close();
        qDebug()<<"SerialClose is ok";
        isopen_status = false;
    }
    disconnect(serialport,SIGNAL(readyRead()),this,SLOT(receivedata()));
    delete serialport;
    serialport = nullptr;
}

/*串口发送数据*/
void SerialCom::writedata(QByteArray data)
{
    if(isopen_status == true)
        send_data_queue->append(data);
}
/*串口接收到数据*/
void SerialCom::receivedata(void)
{
    QByteArray data = this->serialport->readAll();
//    qDebug() << "receive:"<<data;
    emit readdata(data);
}

void SerialCom::SendData_Regularly()
{
    QString str;
    str = "";
    if(send_data_queue->size())
    {
        this->serialport->write(send_data_queue->head());
        str = QString::fromUtf8(send_data_queue->head());
        if(str.startsWith("@HA"))
        {}else{
            qDebug()<<str;
        }
        send_data_queue->pop_front();
        send_data_timer->start(100);
    }else{
        send_data_timer->start(10);
    }

}
