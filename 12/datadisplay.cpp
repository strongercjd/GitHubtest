#include "datadisplay.h"
#include <QDebug>
#include <qregexp.h>
#include <QSet>
#include <QStandardItemModel>
#include <time.h>

#include "MyTabPage.h"
#include <QTabBar>
#include <QTabWidget>

QString num2hex(int num,int len = 4);
QRegExp reg(QString("@\\w{2}(\\w|\\.)+\\*\\n"));//编程语言本身需要转义
DataDisplay::DataDisplay(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    timer_100ms = new QTimer();
    timer_100ms->start(100);
    qDebug()<<"DataDisplay is created";
    is_work = false;
    this->progressBa_quantity->setValue(0);

    connect(widget_serial_port, &SerialCom::readdata, this, &DataDisplay::serial_receive_data);
    connect(widget_serial_port, &SerialCom::StatusChange, this, &DataDisplay::SerialComStatusChange);

    sys_tick_100ms = 0;

    connect(timer_100ms,SIGNAL(timeout()),this,SLOT(timer_100ms_dealwith()));
    connect(this,SIGNAL(Motor_control_stop()),this,SLOT(on_pushButton_assign_clicked()));
    this->spinBox_print->setMinimum(20);
    this->spinBox_print->setMaximum(5000);

    this->comboBox_1->addItem("前进", 0);
    this->comboBox_1->addItem("后退", 1);
    this->comboBox_2->addItem("前进", 0);
    this->comboBox_2->addItem("后退", 1);
    this->comboBox_side_dir->addItem("正转", 0);
    this->comboBox_side_dir->addItem("反转", 1);

    this->comboBox_6->addItem("前进", 0);
    this->comboBox_6->addItem("后退", 1);
    this->comboBox_mop_dir->addItem("正转", 0);
    this->comboBox_mop_dir->addItem("反转", 1);

    this->comboBox_mop_out->addItem("外扩", 0);
    this->comboBox_mop_out->addItem("抬升", 1);

    this->comboBox_roll_dir->addItem("抬升", 0);
    this->comboBox_roll_dir->addItem("下降", 1);

    this->groupBox_senddata->setVisible(false);
    this->pushButton_showsend->setText("打开发送");

    this->comboBox_deebot_type->addItem("法拉第");
    this->comboBox_deebot_type->addItem("法拉第S");
    this->comboBox_deebot_type->addItem("开普勒");
    this->comboBox_deebot_type->addItem("开普勒S");
    this->comboBox_deebot_type->addItem("开普勒SE");
    this->comboBox_deebot_type->addItem("SS");


    this->tabWidget->setDisabled(true);

    customPlotinit();
    m_csvFile = nullptr;
    STATE = WAIT_START;

    Deebot_Type = "法拉第";

    MotorModelInit();
    config_init();
    switch_init(" ");

    // 禁止特定警告
    QDateTime dt = QDateTime::fromTime_t( static_cast<uint>(getDateFromMacro(__DATE__)));

    this->setWindowTitle("地宝分析工具 " + dt.toString("yyyy.MM.dd"));
}
/**
 * @brief getDateFromMacro
 * @param time __DATE__
 * @return
 */
time_t  DataDisplay::getDateFromMacro(char const *time)
{
    char s_month[5];
    int month, day, year;
    struct tm t = {0};
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(time, "%s %d %d", s_month, &day, &year);

    month = (strstr(month_names, s_month)-month_names)/3;

    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_year = year - 1900;
    t.tm_isdst = -1;

    return mktime(&t);
}
void DataDisplay::customPlotinit()
{
    /* 通道线的颜色初始化 */
    uint8_t num = 0;
    line_colors[num++] = QColor ("#fb4934");
    line_colors[num++] = QColor ("#b8bb26");
    line_colors[num++] = QColor ("#fabd2f");
    line_colors[num++] = QColor ("#83a598");
    line_colors[num++] = QColor ("#d3869b");
    line_colors[num++] = QColor ("#8ec07c");
    line_colors[num++] = QColor ("#fe8019");

    line_colors[num++] = QColor ("#cc241d");
    line_colors[num++] = QColor ("#98971a");
    line_colors[num++] = QColor ("#d79921");
    line_colors[num++] = QColor ("#458588");
    line_colors[num++] = QColor ("#b16286");
    line_colors[num++] = QColor ("#689d6a");
    line_colors[num++] = QColor ("#d65d0e");
    line_colors[num++] = QColor ("#0000FF");

    line_colors[num++] = QColor ("#fb4934");
    line_colors[num++] = QColor ("#b8bb26");
    line_colors[num++] = QColor ("#fabd2f");
    line_colors[num++] = QColor ("#83a598");
    line_colors[num++] = QColor ("#d3869b");
    line_colors[num++] = QColor ("#8ec07c");
    line_colors[num++] = QColor ("#fe8019");

    line_colors[num++] = QColor ("#cc241d");
    line_colors[num++] = QColor ("#98971a");
    line_colors[num++] = QColor ("#d79921");
    line_colors[num++] = QColor ("#458588");
    line_colors[num++] = QColor ("#b16286");
    line_colors[num++] = QColor ("#689d6a");
    line_colors[num++] = QColor ("#d65d0e");
    line_colors[num++] = QColor ("#0000FF");

    line_colors[num++] = QColor ("#fb4934");
    line_colors[num++] = QColor ("#b8bb26");
    line_colors[num++] = QColor ("#fabd2f");
    line_colors[num++] = QColor ("#83a598");
    line_colors[num++] = QColor ("#d3869b");
    line_colors[num++] = QColor ("#8ec07c");
    line_colors[num++] = QColor ("#fe8019");

    line_colors[num++] = QColor ("#cc241d");
    line_colors[num++] = QColor ("#98971a");
    line_colors[num++] = QColor ("#d79921");
    line_colors[num++] = QColor ("#458588");
    line_colors[num++] = QColor ("#b16286");
    line_colors[num++] = QColor ("#689d6a");
    line_colors[num++] = QColor ("#d65d0e");
    line_colors[num++] = QColor ("#0000FF");

    num = 0;
    /* 单色UI 深色的UI*/
    gui_colors[num++] = QColor (48,  47,  47,  255);/**<  0: 背景颜色：最黑色 */
    gui_colors[num++] = QColor (80,  80,  80,  255);/**<  1: 网格颜色：中间黑 */
    gui_colors[num++] = QColor (170, 170, 170, 255);/**<  2: 文字颜色：白色 */
    gui_colors[num++] = QColor (48,  47,  47,  200);/**<  3: 背景颜色：黑色，和0的透明度不同 */

    /* 初始化变量 */
    connected =false;//默认没有开始
    plotting =(false);//默认没有绘图
    dataPointNumber =(0);//默认接到0次数据
    channels=(0);//默认0个通道

    /* 设置绘图区 */
    /* 清空绘图区所有内容 */
    this->plot->clearItems();

    /* 设置绘图区背景颜色 */
    this->plot->setBackground (gui_colors[0]);

    /* 设置为高性能模式 */
    this->plot->setNotAntialiasedElements (QCP::aeAll);
    QFont font;
    font.setStyleStrategy (QFont::NoAntialias);
    this->plot->legend->setFont (font);//设置绘图区字体


    /* 设置X轴风格 */
    this->plot->xAxis->grid()->setPen (QPen(gui_colors[2], 1, Qt::DotLine));
    this->plot->xAxis->grid()->setSubGridPen (QPen(gui_colors[1], 1, Qt::DotLine));
    this->plot->xAxis->grid()->setSubGridVisible (true);
    this->plot->xAxis->setBasePen (QPen (gui_colors[2]));
    this->plot->xAxis->setTickPen (QPen (gui_colors[2]));
    this->plot->xAxis->setSubTickPen (QPen (gui_colors[2]));
    this->plot->xAxis->setUpperEnding (QCPLineEnding::esSpikeArrow);
    this->plot->xAxis->setTickLabelColor (gui_colors[2]);
    this->plot->xAxis->setTickLabelFont (font);
    /* 显示范围 */
    this->plot->xAxis->setRange (dataPointNumber - this->spinPoints->value(), dataPointNumber);

    /* 设置Y轴风格 */
    this->plot->yAxis->grid()->setPen (QPen(gui_colors[2], 1, Qt::DotLine));
    this->plot->yAxis->grid()->setSubGridPen (QPen(gui_colors[1], 1, Qt::DotLine));
    this->plot->yAxis->grid()->setSubGridVisible (true);
    this->plot->yAxis->setBasePen (QPen (gui_colors[2]));
    this->plot->yAxis->setTickPen (QPen (gui_colors[2]));
    this->plot->yAxis->setSubTickPen (QPen (gui_colors[2]));
    this->plot->yAxis->setUpperEnding (QCPLineEnding::esSpikeArrow);
    this->plot->yAxis->setTickLabelColor (gui_colors[2]);
    this->plot->yAxis->setTickLabelFont (font);
    /* Range */
    //ui->plot->yAxis->setRange (ui->spinAxesMin->value(), ui->spinAxesMax->value());
    /* User can change Y axis tick step with a spin box */
    //ui->plot->yAxis->setAutoTickStep (false);
    //ui->plot->yAxis->(ui->spinYStep->value());

    /* User interactions Drag and Zoom are allowed only on X axis, Y is fixed manually by UI control */
    this->plot->setInteraction (QCP::iRangeDrag, true);
    //ui->plot->setInteraction (QCP::iRangeZoom, true);
    this->plot->setInteraction (QCP::iSelectPlottables, true);
    this->plot->setInteraction (QCP::iSelectLegend, true);
    this->plot->axisRect()->setRangeDrag (Qt::Horizontal);
    this->plot->axisRect()->setRangeZoom (Qt::Horizontal);

    /* 设置图例 */
    QFont legendFont;
    legendFont.setPointSize (9);
    this->plot->legend->setVisible (true);
    this->plot->legend->setFont (legendFont);
    this->plot->legend->setBrush (gui_colors[3]);
    this->plot->legend->setBorderPen (gui_colors[2]);
    /* By default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement */
    this->plot->axisRect()->insetLayout()->setInsetAlignment (0, Qt::AlignTop|Qt::AlignRight);

    /* 鼠标滚轮在绘图区滚动槽函数 */
    connect (this->plot, SIGNAL (mouseWheel (QWheelEvent*)), this, SLOT (on_mouse_wheel_in_plot (QWheelEvent*)));

    /* 打印鼠标在绘图区中坐标槽函数 */
    connect (this->plot, SIGNAL (mouseMove (QMouseEvent*)), this, SLOT (onMouseMoveInPlot (QMouseEvent*)));

    /* 鼠标选择图例或绘图区中曲线的槽函数 */
    connect (this->plot, SIGNAL(selectionChangedByUser()), this, SLOT(channel_selection()));
    /* 鼠标双击图例的槽函数 */
    connect (this->plot, SIGNAL(legendDoubleClick (QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)), this, SLOT(legend_double_click (QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)));

    /* 定时刷新绘图区 */
    connect (&updateTimer, SIGNAL (timeout()), this, SLOT (replot()));

    /*向绘图区增加新的数据槽函数*/
    connect (this, SIGNAL(newData(QStringList)), this, SLOT(onNewDataArrived(QStringList)));
}
void DataDisplay::TimeToPlot()
{
    QStringList incomingData;
    QString receivedDataTemp;
    receivedDataTemp.clear(); //清空数据
    double value0 = (static_cast<double>(gyro_value))/100;
    double value1 = (static_cast<double>(yaw_value))/100;
    double Difference_Value;
    Difference_Value = (value1 - value0);
    if(Difference_Value < -180)
    {
        Difference_Value = 360 + Difference_Value;
    }else{
        if(Difference_Value > 180)
            Difference_Value = -360 + Difference_Value;
    }
    if(Plot_Data[Epson_Yaw].Enable == Channel_Enable)
        Plot_Data[Epson_Yaw].value_data  = QString("%1").arg(Difference_Value);
    for (uint8_t i=0;i<All_Channel_num;i++)
    {
        QString Plot_Channel_Data;
        Plot_Channel_Data = "";
        for (uint8_t n=0;n<Plot_data_all;n++)
        {
            if((Plot_Data[n].Channel_num == i)&&(Plot_Data[n].Enable == Channel_Enable))
            {
                Plot_Channel_Data = Plot_Data[n].value_data;
                n = Plot_data_all;
            }
        }
        receivedDataTemp.append(Plot_Channel_Data);
        if(i<All_Channel_num-1)
            receivedDataTemp.append(' ');
    }
    if(All_Channel_num)
    {
        incomingData = receivedDataTemp.split(' '); //使用空格将它们分割
        emit newData(incomingData); //发送信号，解析到数据用于显示到绘图区
        saveStream(incomingData);
    }
}
void DataDisplay::timer_100ms_dealwith()
{
    sys_tick_100ms++;
    if(sys_tick_100ms%10==0)
    {
        current_time = QTime::currentTime();
        QString now_time = current_time.toString();
        this->label_83->setText(now_time);
        serialport_writedata("@HA*\n");
    }
    this->timer_100ms->start(100);
}

void DataDisplay::MotorModelInit()
{
    QStringList list_names = {"设备名", "当前值", "设定值", "PWM(%)", "电流(mA)"};
    QStringList motor_names;
    motor_names << "左轮"<< "右轮"<< "左边刷"<< "右边刷"<<"滚刷"<<"风机"<< "清水泵"<< "左拖地电机"<< "右拖地电机"<< "外扩电机";
    TypeList.clear();
    TypeList << "开普勒"<< "开普勒S" << "开普勒SE";
    if(TypeList.contains(Deebot_Type))
    {
        motor_names.clear();
        motor_names << "左轮"<< "右轮"<< "右边刷"<<"滚刷"<<"风机"<< "清水泵"<< "滚筒电机"<< "外扩电机"<<"污水泵";
    }
    QItemSelectionModel *m = tableView_motor->selectionModel();
    this->motor_model  = new QStandardItemModel(motor_names.size(),list_names.size());
    for(int i = 0; i < motor_names.size();i++)
    {
        this->motor_model->setItem(i,0,(new QStandardItem(motor_names[i])));
    }
    this->motor_model->setHorizontalHeaderLabels(list_names);
    this->tableView_motor->setModel(this->motor_model);
    delete m;
}

void DataDisplay::config_init()
{
    QStringList config_names = {"核心板配置", "超声波", "电池容量", "风机", "水泵类型"};
    QStringList list_names = {"名称", "状态"};
    this->config_model  = new QStandardItemModel(config_names.size(),list_names.size());
    for(int i = 0; i < config_names.size();i++)
    {
        this->config_model->setItem(i,0,(new QStandardItem(config_names[i])));
    }
    this->config_model->setHorizontalHeaderLabels(list_names);
    this->tableView_config->setModel(this->config_model);
}

void DataDisplay::switch_init(QString dev_type)
{
    QStringList switch_names;
    QStringList list_names;
    switch_names << "尘盒" << "地毯" << "水箱" << "抹盘检测" << "抹盘上升" << "满水检测" << "外扩检测";
    if(Deebot_Type == "开普勒")
    {
        switch_names.clear();
        switch_names << "尘盒" << "地毯" << "边刷状态" << "接水盘" << "污水安装" << "污水水满" << "滚筒安装"  << "外扩检测"<<"超声波AD"<<"污水箱AD"<<"清水箱水量";
    }
    if(Deebot_Type == "开普勒S")
    {
        switch_names.clear();
        switch_names << "尘盒" << "地毯" << "边刷状态" << "接水盘" << "污水安装" << "污水水满" << "滚筒安装"  << "外扩检测"<<"超声波AD"<<"污水箱AD"<<"清水箱水量"<<"滚刷抬升";
    }
    if(Deebot_Type == "开普勒SE")
    {
        switch_names.clear();
        switch_names << "尘盒" << "地毯" << "接水盘" << "污水安装" << "污水水满" << "滚筒安装"  << "外扩检测"<<"超声波AD"<<"污水箱AD"<<"清水箱水量"<<"IMU型号";
    }
    list_names << "名称" << "状态";

    QItemSelectionModel *m = tableView_switch->selectionModel();
    this->switch_model  = new QStandardItemModel(switch_names.size(),list_names.size());
    for(int i = 0; i < switch_names.size();i++)
    {
        this->switch_model->setItem(i,0,(new QStandardItem(switch_names[i])));
    }
    this->switch_model->setHorizontalHeaderLabels(list_names);
    this->tableView_switch->setModel(this->switch_model);
    delete m;
}
QSet<QString> DataDisplay::getAllMatchResults(QString &text ,const QRegExp &regexp)
{
    QSet<QString> resultSet;
    int pos = 0;
    while((pos = regexp.indexIn(text,pos)) != -1)
    {
        pos += regexp.matchedLength();
        resultSet <<regexp.cap(0);
    }
    return resultSet;
}
void DataDisplay::SerialComStatusChange(bool status)
{
    if(status)
    {
        this->comboBox_deebot_type->setDisabled(true);
        this->tabWidget->setDisabled(false);
    }else{
        connected = false;
        plotting = false;
        this->comboBox_deebot_type->setDisabled(false);
        this->tabWidget->setDisabled(true);
    }
}
void DataDisplay::serial_receive_data(QByteArray data)
{
    this->byte_info += QString(data);
    if(radioButton_enablereceive->isChecked())
    {
        receivedata_textBrowser->insertPlainText(QString(data));
        receivedata_textBrowser->moveCursor(QTextCursor::End);
    }
    QSet<QString> temp = getAllMatchResults(this->byte_info,reg);
    for(auto str : temp)
    {
        protocol_parser(str);
        this->byte_info =  this->byte_info.replace(str,"");
    }
    if(this->byte_info.size() > 1000)
    {
        qDebug()<<"内存中未匹配字符超过限制"<<this->byte_info.size();
        this->byte_info =  "";
    }
}
/**
 * @brief 通道数处理，主要用于虚拟示波器显示那些通道
 */
uint8_t DataDisplay::Deal_Chanel_Value()
{
    uint8_t Channel_num;
    Channel_num = 0;
    if(this->cB_driverwheel->isChecked() == true)
    {
        Plot_Data[Left_Driver_Current].Name_String = "左轮电流";
        Plot_Data[Left_Driver_Current].Enable = Channel_Enable;
        Plot_Data[Left_Driver_Current].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Driver_current_speed].Name_String =  "左轮当前速度";
        Plot_Data[Left_Driver_current_speed].Enable = Channel_Enable;
        Plot_Data[Left_Driver_current_speed].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Driver_target_speed].Name_String =  "左轮设置速度";
        Plot_Data[Left_Driver_target_speed].Enable = Channel_Enable;
        Plot_Data[Left_Driver_target_speed].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Driver_PWM].Name_String =  "左轮占空比";
        Plot_Data[Left_Driver_PWM].Enable = Channel_Enable;
        Plot_Data[Left_Driver_PWM].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Driver_Current].Name_String =  "右轮电流";
        Plot_Data[Right_Driver_Current].Enable = Channel_Enable;
        Plot_Data[Right_Driver_Current].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Driver_current_speed].Name_String =  "右轮当前速度";
        Plot_Data[Right_Driver_current_speed].Enable = Channel_Enable;
        Plot_Data[Right_Driver_current_speed].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Driver_target_speed].Name_String =  "右轮设置速度";
        Plot_Data[Right_Driver_target_speed].Enable = Channel_Enable;
        Plot_Data[Right_Driver_target_speed].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Driver_PWM].Name_String =  "右轮占空比";
        Plot_Data[Right_Driver_PWM].Enable = Channel_Enable;
        Plot_Data[Right_Driver_PWM].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Left_Driver_Current].Enable = Channel_Disenabled;
        Plot_Data[Left_Driver_current_speed].Enable = Channel_Disenabled;
        Plot_Data[Left_Driver_target_speed].Enable = Channel_Disenabled;
        Plot_Data[Left_Driver_PWM].Enable = Channel_Disenabled;
        Plot_Data[Right_Driver_Current].Enable = Channel_Disenabled;
        Plot_Data[Right_Driver_current_speed].Enable = Channel_Disenabled;
        Plot_Data[Right_Driver_target_speed].Enable = Channel_Disenabled;
        Plot_Data[Right_Driver_PWM].Enable = Channel_Disenabled;
    }

    if(this->cB_sidebrush->isChecked() == true)
    {        
        Plot_Data[Left_Brust_Current].Name_String =  "左边刷电流";
        Plot_Data[Left_Brust_Current].Enable = Channel_Enable;
        Plot_Data[Left_Brust_Current].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Brust_voltage].Name_String = "左边刷电压";
        Plot_Data[Left_Brust_voltage].Enable = Channel_Enable;
        Plot_Data[Left_Brust_voltage].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Brust_pwm].Name_String = "左边刷占空比";
        Plot_Data[Left_Brust_pwm].Enable = Channel_Enable;
        Plot_Data[Left_Brust_pwm].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Brust_Current].Name_String = "右边刷电流";
        Plot_Data[Right_Brust_Current].Enable = Channel_Enable;
        Plot_Data[Right_Brust_Current].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Brust_voltage].Name_String = "右边刷电压";
        Plot_Data[Right_Brust_voltage].Enable = Channel_Enable;
        Plot_Data[Right_Brust_voltage].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Right_Brust_pwm].Name_String = "右边刷占空比";
        Plot_Data[Right_Brust_pwm].Enable = Channel_Enable;
        Plot_Data[Right_Brust_pwm].Channel_num = Channel_num;
        Channel_num ++;
    }else{
        Plot_Data[Left_Brust_Current].Enable = Channel_Disenabled;
        Plot_Data[Left_Brust_voltage].Enable = Channel_Disenabled;
        Plot_Data[Left_Brust_pwm].Enable = Channel_Disenabled;
        Plot_Data[Right_Brust_Current].Enable = Channel_Disenabled;
        Plot_Data[Right_Brust_voltage].Enable = Channel_Disenabled;
        Plot_Data[Right_Brust_pwm].Enable = Channel_Disenabled;
    }


    if(this->cB_Rollcur->isChecked() == true)
    {
        Plot_Data[Roll_Current].Name_String =  "滚刷电流";
        Plot_Data[Roll_Current].Enable = Channel_Enable;
        Plot_Data[Roll_Current].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Roll_voltage].Name_String =  "滚刷电压";
        Plot_Data[Roll_voltage].Enable = Channel_Enable;
        Plot_Data[Roll_voltage].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Roll_pwm].Name_String =  "滚刷占空比";
        Plot_Data[Roll_pwm].Enable = Channel_Enable;
        Plot_Data[Roll_pwm].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Roll_Current].Enable = Channel_Disenabled;
        Plot_Data[Roll_voltage].Enable = Channel_Disenabled;
        Plot_Data[Roll_pwm].Enable = Channel_Disenabled;
    }


    if(this->cB_mopcur->isChecked() == true)
    {
        Plot_Data[Left_Mop_Currnet].Enable = Channel_Enable;    
        Plot_Data[Left_Mop_Currnet].Name_String = "左拖布电流";
        Plot_Data[Left_Mop_Currnet].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Mop_voltage].Enable = Channel_Enable;

        Plot_Data[Left_Mop_voltage].Name_String = "左拖布转速";
 
        Plot_Data[Left_Mop_voltage].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Left_Mop_pwm].Enable = Channel_Enable;

        Plot_Data[Left_Mop_pwm].Name_String = "左拖布占空比";

        Plot_Data[Left_Mop_pwm].Channel_num = Channel_num;
        Channel_num ++;

            Plot_Data[Right_Mop_Currnet].Name_String =  "右拖布电流";
            Plot_Data[Right_Mop_Currnet].Enable = Channel_Enable;
            Plot_Data[Right_Mop_Currnet].Channel_num = Channel_num;
            Channel_num ++;
            Plot_Data[Right_Mop_voltage].Name_String =  "右拖布速度";

            Plot_Data[Right_Mop_voltage].Enable = Channel_Enable;
            Plot_Data[Right_Mop_voltage].Channel_num = Channel_num;
            Channel_num ++;

            Plot_Data[Right_Mop_pwm].Name_String =  "右拖布占空比";
            Plot_Data[Right_Mop_pwm].Enable = Channel_Enable;
            Plot_Data[Right_Mop_pwm].Channel_num = Channel_num;
            Channel_num ++;

    }else{        
        Plot_Data[Left_Mop_Currnet].Enable = Channel_Disenabled;
        Plot_Data[Left_Mop_voltage].Enable = Channel_Disenabled;
        Plot_Data[Left_Mop_pwm].Enable = Channel_Disenabled;
        Plot_Data[Right_Mop_Currnet].Enable = Channel_Disenabled;
        Plot_Data[Right_Mop_voltage].Enable = Channel_Disenabled;
        Plot_Data[Right_Mop_pwm].Enable = Channel_Disenabled;
    }

    if(this->cB_Gyro->isChecked() == true)
    {        
        Plot_Data[Gyro_Epson].Name_String =   "单轴";
        Plot_Data[Gyro_Epson].Enable = Channel_Enable;
        Plot_Data[Gyro_Epson].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Gyro_Roll].Name_String =   "Roll";
        Plot_Data[Gyro_Roll].Enable = Channel_Enable;
        Plot_Data[Gyro_Roll].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Gyro_Pitch].Name_String =   "Pitch";
        Plot_Data[Gyro_Pitch].Enable = Channel_Enable;
        Plot_Data[Gyro_Pitch].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Gyro_Yaw].Name_String =   "Yaw";
        Plot_Data[Gyro_Yaw].Enable = Channel_Enable;
        Plot_Data[Gyro_Yaw].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Gyro_Epson].Enable = Channel_Disenabled;
        Plot_Data[Gyro_Roll].Enable = Channel_Disenabled;
        Plot_Data[Gyro_Pitch].Enable = Channel_Disenabled;
        Plot_Data[Gyro_Yaw].Enable = Channel_Disenabled;
    }

    if(this->cB_Acc->isChecked() == true)
    {
        Plot_Data[Acc_X].Name_String =   "AccX";
        Plot_Data[Acc_X].Enable = Channel_Enable;
        Plot_Data[Acc_X].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Acc_Y].Name_String =   "AccY";
        Plot_Data[Acc_Y].Enable = Channel_Enable;
        Plot_Data[Acc_Y].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Acc_Z].Name_String =   "AccZ";
        Plot_Data[Acc_Z].Enable = Channel_Enable;
        Plot_Data[Acc_Z].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Acc_X].Enable = Channel_Disenabled;
        Plot_Data[Acc_Y].Enable = Channel_Disenabled;
        Plot_Data[Acc_Z].Enable = Channel_Disenabled;
    }

    if(this->cB_Yanbian->isChecked() == true)
    {
            Plot_Data[YanBian_F].Name_String =   "前沿边";
            Plot_Data[YanBian_F].Enable = Channel_Enable;
            Plot_Data[YanBian_F].Channel_num = Channel_num;
            Channel_num ++;

            Plot_Data[YanBian_B].Name_String =   "后沿边";
            Plot_Data[YanBian_B].Enable = Channel_Enable;
            Plot_Data[YanBian_B].Channel_num = Channel_num;
            Channel_num ++;
    }else{
        Plot_Data[YanBian_F].Enable = Channel_Disenabled;
        Plot_Data[YanBian_B].Enable = Channel_Disenabled;
    }

    if(this->cB_Epson_Yaw->isChecked() == true)
    {
        Plot_Data[Epson_Yaw].Name_String =   "单轴-6轴";
        Plot_Data[Epson_Yaw].Enable = Channel_Enable;
        Plot_Data[Epson_Yaw].Channel_num = Channel_num;
        Channel_num ++;
    }else{
        Plot_Data[Epson_Yaw].Enable = Channel_Disenabled;
    }

    if(this->cB_Bump->isChecked() == true)
    {
        Plot_Data[Bump_Plot].Name_String =   "撞板";
        Plot_Data[Bump_Plot].Enable = Channel_Enable;
        Plot_Data[Bump_Plot].Channel_num = Channel_num;
        Channel_num ++;
    }else{
        Plot_Data[Bump_Plot].Enable = Channel_Disenabled;
    }

    if(this->cB_Drop->isChecked() == true)
    {
        Plot_Data[Drop_Plot].Name_String =   "跌落";
        Plot_Data[Drop_Plot].Enable = Channel_Enable;
        Plot_Data[Drop_Plot].Channel_num = Channel_num;
        Channel_num ++;
    }else{
        Plot_Data[Drop_Plot].Enable = Channel_Disenabled;
    }

    if(this->cB_Down->isChecked() == true)
    {
        Plot_Data[Ldown_Plot].Name_String =   "左下视";
        Plot_Data[Ldown_Plot].Enable = Channel_Enable;
        Plot_Data[Ldown_Plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[FLdown_Plot].Name_String =   "左前下视";
        Plot_Data[FLdown_Plot].Enable = Channel_Enable;
        Plot_Data[FLdown_Plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[FRdown_Plot].Name_String =   "右前下视";
        Plot_Data[FRdown_Plot].Enable = Channel_Enable;
        Plot_Data[FRdown_Plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Rdown_Plot].Name_String =   "右下视";
        Plot_Data[Rdown_Plot].Enable = Channel_Enable;
        Plot_Data[Rdown_Plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[BR_down_Plot].Name_String =   "右后下视";
        Plot_Data[BR_down_Plot].Enable = Channel_Enable;
        Plot_Data[BR_down_Plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[BL_down_Plot].Name_String =   "左后下视";
        Plot_Data[BL_down_Plot].Enable = Channel_Enable;
        Plot_Data[BL_down_Plot].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Ldown_Plot].Enable = Channel_Disenabled;
        Plot_Data[FLdown_Plot].Enable = Channel_Disenabled;
        Plot_Data[FRdown_Plot].Enable = Channel_Disenabled;
        Plot_Data[Rdown_Plot].Enable = Channel_Disenabled;
        Plot_Data[BR_down_Plot].Enable = Channel_Disenabled;
        Plot_Data[BL_down_Plot].Enable = Channel_Disenabled;
    }

    if(this->cB_dattery->isChecked() == true)
    {
        Plot_Data[Battery_percent_plot].Name_String =   "电量百分比";
        Plot_Data[Battery_percent_plot].Enable = Channel_Enable;
        Plot_Data[Battery_percent_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battrer_voltage_plot].Name_String =   "电池电压";
        Plot_Data[Battrer_voltage_plot].Enable = Channel_Enable;
        Plot_Data[Battrer_voltage_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battery_charge_voltage_plot].Name_String =   "充电座电压";
        Plot_Data[Battery_charge_voltage_plot].Enable = Channel_Enable;
        Plot_Data[Battery_charge_voltage_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battery_current_plot].Name_String =   "电池电流";
        Plot_Data[Battery_current_plot].Enable = Channel_Enable;
        Plot_Data[Battery_current_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battery_temp_plot].Name_String =   "电池温度";
        Plot_Data[Battery_temp_plot].Enable = Channel_Enable;
        Plot_Data[Battery_temp_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battery_step_plot].Name_String =   "充电阶段";
        Plot_Data[Battery_step_plot].Enable = Channel_Enable;
        Plot_Data[Battery_step_plot].Channel_num = Channel_num;
        Channel_num ++;

        Plot_Data[Battery_sandard_current_plot].Name_String =    "基准电流";
        Plot_Data[Battery_sandard_current_plot].Enable = Channel_Enable;
        Plot_Data[Battery_sandard_current_plot].Channel_num = Channel_num;
        Channel_num ++;
    }else{        
        Plot_Data[Battery_percent_plot].Enable = Channel_Disenabled;
        Plot_Data[Battrer_voltage_plot].Enable = Channel_Disenabled;
        Plot_Data[Battery_charge_voltage_plot].Enable = Channel_Disenabled;
        Plot_Data[Battery_current_plot].Enable = Channel_Disenabled;
        Plot_Data[Battery_temp_plot].Enable = Channel_Disenabled;
        Plot_Data[Battery_step_plot].Enable = Channel_Disenabled;
        Plot_Data[Battery_sandard_current_plot].Enable = Channel_Disenabled;
    }

    All_Channel_num = Channel_num;
    if(Channel_num > CUSTOM_LINE_COLORS)
        return 0;
    else
        return 1;
}
void DataDisplay::on_pushButton_start_clicked()
{
    if(widget_serial_port->isopen_status)
    {
        if(this->is_work == false)
        {
            for (uint8_t n=0;n<Plot_data_all;n++)
            {
                Plot_Data[n].Enable = Channel_Disenabled;
                Plot_Data[n].value_data = "";
            }
            if(Deal_Chanel_Value())
            {
                this->is_work = true;
                this->pushButton_start->setStyleSheet("border-image: url(:/newPrefix/resource/stop_3.png)");
                serialport_writedata("@WYY*\n");//开始回读
                if(this->radioButton_RecordData->isChecked())//是否要保存数据到csv文件
                    openCsvFile();
                this->radioButton_RecordData->setDisabled(true);
                this->groupBox_4->setDisabled(true);

                this->plot->clearPlottables();
                this->listWidget_Channels->clear();
                channels = 0;
                dataPointNumber = 0;
                customPlotinit();
                this->plot->replot();

                updateTimer.start (20); //20ms重新绘制
                connected = true; //正在连接flg
//                plotting = true;//正在绘图flg
            }
        }
        else
        {
            serialport_writedata("@FFW*\n");//停止回读
            this->is_work = false;
            this->pushButton_start->setStyleSheet("border-image: url(:/newPrefix/resource/play_1.png)");
            if(this->radioButton_RecordData->isChecked())//是否要保存数据到csv文件
                closeCsvFile();
            this->radioButton_RecordData->setDisabled(false);
            this->groupBox_4->setDisabled(false);
        }
    }

}

double DataDisplay::signed_convert(QString &str)
{
    int num = 0;
    if(str[0] > "7")
    {
        num = str.toInt(nullptr,16) - pow(16,str.size());
        return static_cast<double>(num);
    }
    else
        return static_cast<double>(str.toInt(nullptr,16));
}

void DataDisplay::update_switch_Item(QString name,QString str)
{
    for (int row = 0; row < switch_model->rowCount(); ++row)
    {
        QModelIndex index = switch_model->index(row, 0);
        if (index.data().toString() == name) {
            QStandardItem* item_temp = new QStandardItem(str);
            this->switch_model->setItem(row, 1, item_temp);
            break;
        }
    }
}
void DataDisplay::update_Motor_Item(QString row_name,QString col_name,QString str)
{
    uint8_t row,col,row_flg,col_flg;
    row = 0;
    col = 0;
    row_flg = 0;
    col_flg = 0;
    for (row = 0; row < motor_model->rowCount(); ++row)
    {
        QModelIndex index = motor_model->index(row, 0);
        if (index.data().toString() == row_name) {
            row_flg = 1;
            break;
        }
    }
    for (col = 0; col < motor_model->columnCount(); ++col)
    {
        QString headerText = motor_model->headerData(col, Qt::Horizontal).toString();
        if (headerText == col_name) {
            col_flg = 1;
            break;
        }
    }
    if((row_flg)&&(col_flg))
    {
        QStandardItem* item_temp = new QStandardItem(str);
        this->motor_model->setItem(row, col, item_temp);
    }
}
#define DOWN_LEFT_MASK    (0x20)
#define DOWN_FRONT_MASK   (0x10)
#define DOWN_RIGHT_MASK   (0x08)
#define DOWN_BACK_RIGHT_MASK   (0x02)
#define DOWN_BACK_LEFT_MASK   (0x04)

void DataDisplay::protocol_parser(QString &msg)
{
    if(msg.startsWith("@AB"))//驱动轮
    {
        int Lspeed = msg.mid(5,2).toInt(nullptr,16);
        int Lsetspeed = msg.mid(7,2).toInt(nullptr,16);
        int Lpwm = msg.mid(9,2).toInt(nullptr,16);
        int Lcurrent = msg.mid(11,4).toInt(nullptr,16);
        QString str_Lspeed = QString::number(Lspeed);
        QString str_Lsetspeed = QString::number(Lsetspeed);
        QString str_Lpwm = QString::number(Lpwm);
        QString str_Lcurrent = QString::number(Lcurrent);//左驱动轮电流
        if(Plot_Data[Left_Driver_Current].Enable == Channel_Enable)
            Plot_Data[Left_Driver_Current].value_data  = str_Lcurrent;
        if(Plot_Data[Left_Driver_current_speed].Enable == Channel_Enable)
        {
            if(msg.mid(3,2) == "01")//前进
                Plot_Data[Left_Driver_current_speed].value_data  = str_Lspeed;
            else
                Plot_Data[Left_Driver_current_speed].value_data  = "-"+str_Lspeed;
        }

        if(Plot_Data[Left_Driver_target_speed].Enable == Channel_Enable)
        {
            if(msg.mid(3,2) == "01")//前进
                Plot_Data[Left_Driver_target_speed].value_data  = str_Lsetspeed;
            else
                Plot_Data[Left_Driver_target_speed].value_data  = "-"+str_Lsetspeed;
        }
        if(Plot_Data[Left_Driver_PWM].Enable == Channel_Enable)
            Plot_Data[Left_Driver_PWM].value_data  = str_Lpwm;

        update_Motor_Item("左轮","当前值",str_Lspeed);
        update_Motor_Item("左轮","设定值",str_Lsetspeed);
        update_Motor_Item("左轮","PWM(%)",str_Lpwm);
        update_Motor_Item("左轮","电流(mA)",str_Lcurrent);

        int Rspeed = msg.mid(17,2).toInt(nullptr,16);
        int Rsetspeed = msg.mid(19,2).toInt(nullptr,16);
        int Rpwm = msg.mid(21,2).toInt(nullptr,16);
        int Rcurrent = msg.mid(23,4).toInt(nullptr,16);
        QString str_Rspeed = QString::number(Rspeed);
        QString str_Rsetspeed = QString::number(Rsetspeed);
        QString str_Rpwm = QString::number(Rpwm);
        QString str_Rcurrent = QString::number(Rcurrent);//右边驱动轮电流        
        if(Plot_Data[Right_Driver_Current].Enable == Channel_Enable)
            Plot_Data[Right_Driver_Current].value_data  = str_Rcurrent;
        if(Plot_Data[Right_Driver_current_speed].Enable == Channel_Enable)
        {
            if(msg.mid(15,2) == "01")//前进
                Plot_Data[Right_Driver_current_speed].value_data  = str_Rspeed;
            else
                Plot_Data[Right_Driver_current_speed].value_data  = "-"+str_Rspeed;
        }
        if(Plot_Data[Right_Driver_target_speed].Enable == Channel_Enable)
        {
            if(msg.mid(15,2) == "01")//前进
                Plot_Data[Right_Driver_target_speed].value_data  = str_Rsetspeed;
            else
                Plot_Data[Right_Driver_target_speed].value_data  =  "-"+str_Rsetspeed;
        }
        if(Plot_Data[Right_Driver_PWM].Enable == Channel_Enable)
            Plot_Data[Right_Driver_PWM].value_data  = str_Rpwm;

        update_Motor_Item("右轮","当前值",str_Rspeed);
        update_Motor_Item("右轮","设定值",str_Rsetspeed);
        update_Motor_Item("右轮","PWM(%)",str_Rpwm);
        update_Motor_Item("右轮","电流(mA)",str_Rcurrent);
    }
    if(msg.startsWith("@AD"))//边刷
    {
        int Lvoltage =msg.mid(3,2).toInt(nullptr,16) / 10;
        int Lpwm =msg.mid(5,2).toInt(nullptr,16) ;
        int Lcurrent =msg.mid(7,4).toInt(nullptr,16) ;
        int Rvoltage =msg.mid(11,2).toInt(nullptr,16) / 10;
        int Rpwm =msg.mid(13,2).toInt(nullptr,16) ;
        int Rcurrent =msg.mid(15,4).toInt(nullptr,16) ;

        QString str_Lvoltage = QString::number(Lvoltage);
        QString str_Lpwm = QString::number(Lpwm);
        QString str_Lcurrent = QString::number(Lcurrent);
        QString str_Rvoltage = QString::number(Rvoltage);
        QString str_Rpwm = QString::number(Rpwm);
        QString str_Rcurrent = QString::number(Rcurrent);

        if(Plot_Data[Left_Brust_Current].Enable == Channel_Enable)
            Plot_Data[Left_Brust_Current].value_data  = str_Lcurrent;
        if(Plot_Data[Left_Brust_voltage].Enable == Channel_Enable)
            Plot_Data[Left_Brust_voltage].value_data  = str_Lvoltage;
        if(Plot_Data[Left_Brust_pwm].Enable == Channel_Enable)
            Plot_Data[Left_Brust_pwm].value_data  = str_Lpwm;

        if(Plot_Data[Right_Brust_Current].Enable == Channel_Enable)
            Plot_Data[Right_Brust_Current].value_data  = str_Rcurrent;
        if(Plot_Data[Right_Brust_voltage].Enable == Channel_Enable)
            Plot_Data[Right_Brust_voltage].value_data  = str_Rvoltage;
        if(Plot_Data[Right_Brust_pwm].Enable == Channel_Enable)
            Plot_Data[Right_Brust_pwm].value_data  = str_Rpwm;

        double data_f;
        data_f = (static_cast<double>(msg.mid(3,2).toInt(nullptr,16)))/10;
        update_Motor_Item("左边刷","设定值",QString::number(data_f,'f',1));
        update_Motor_Item("左边刷","PWM(%)",str_Lpwm);
        update_Motor_Item("左边刷","电流(mA)",str_Lcurrent);

        data_f = (static_cast<double>(msg.mid(11,2).toInt(nullptr,16)))/10;

        update_Motor_Item("右边刷","设定值",QString::number(data_f,'f',1));
        update_Motor_Item("右边刷","PWM(%)",str_Rpwm);
        update_Motor_Item("右边刷","电流(mA)",str_Rcurrent);
    }
    if(msg.startsWith("@AH"))//风机
    {
        int speed;
        int pwm;
        speed =msg.mid(3,2).toInt(nullptr,16);
        pwm =msg.mid(5,2).toInt(nullptr,16);
        TypeList.clear();
        TypeList << "开普勒" <<"开普勒S" << "开普勒SE"<<"法拉第S" << "SS";
        if(TypeList.contains(Deebot_Type))
        {
            speed =msg.mid(3,2).toInt(nullptr,16);
            pwm =msg.mid(5,2).toInt(nullptr,16);
            speed = speed | (pwm<<8);
            pwm =msg.mid(7,2).toInt(nullptr,16);
        }
        update_Motor_Item("风机","当前值",QString::number(speed));
        update_Motor_Item("风机","PWM(%)",QString::number(pwm));
    }
    if(msg.startsWith("@AI"))//下视
    {
        QString left_back = QString::number(msg.mid(3,4).toInt(nullptr,16));
        QString left = QString::number(msg.mid(7,4).toInt(nullptr,16));
        QString left_front = QString::number(msg.mid(11,4).toInt(nullptr,16));
        QString right_front = QString::number(msg.mid(15,4).toInt(nullptr,16));
        QString right = QString::number(msg.mid(19,4).toInt(nullptr,16));
        QString right_back = QString::number(msg.mid(23,4).toInt(nullptr,16));     

        if(Plot_Data[Ldown_Plot].Enable == Channel_Enable)
            Plot_Data[Ldown_Plot].value_data  = left;
        if(Plot_Data[FLdown_Plot].Enable == Channel_Enable)
            Plot_Data[FLdown_Plot].value_data  = left_front;
        if(Plot_Data[FRdown_Plot].Enable == Channel_Enable)
            Plot_Data[FRdown_Plot].value_data  = right_front;
        if(Plot_Data[Rdown_Plot].Enable == Channel_Enable)
            Plot_Data[Rdown_Plot].value_data  = right;
        if(Plot_Data[BR_down_Plot].Enable == Channel_Enable)
            Plot_Data[BR_down_Plot].value_data  = right_back;
        if(Plot_Data[BL_down_Plot].Enable == Channel_Enable)
            Plot_Data[BL_down_Plot].value_data  = left_back;
        /*
        #define DOWN_LEFT_MASK    (0x20)
        #define DOWN_FRONT_MASK   (0x10)
        #define DOWN_RIGHT_MASK   (0x08)
        #define DOWN_BACK_RIGHT_MASK   (0x02)
        #define DOWN_BACK_LEFT_MASK   (0x04)
        */
        int data = msg.mid(27,2).toInt(nullptr,16);

        if(data & (DOWN_LEFT_MASK))
            this->label_Ldown->setStyleSheet("background:red");
        else
            this->label_Ldown->setStyleSheet("");
        this->label_Ldown->setText(left);

        if(data & (DOWN_FRONT_MASK))
            this->label_FLdown->setStyleSheet("background:red");
        else
            this->label_FLdown->setStyleSheet("");
        this->label_FLdown->setText(left_front);

        if(data & (DOWN_FRONT_MASK))
            this->label_FRdown->setStyleSheet("background:red");
        else
            this->label_FRdown->setStyleSheet("");
        this->label_FRdown->setText(right_front);

        if(data & (DOWN_RIGHT_MASK))
            this->label_Rdown->setStyleSheet("background:red");
        else
            this->label_Rdown->setStyleSheet("");
        this->label_Rdown->setText(right);

        if(data & (DOWN_BACK_RIGHT_MASK))
            this->label_BRdown->setStyleSheet("background:red");
        else
            this->label_BRdown->setStyleSheet("");
        this->label_BRdown->setText(right_back);

        if(data & (DOWN_BACK_LEFT_MASK))
            this->label_BLdown->setStyleSheet("background:red");
        else
            this->label_BLdown->setStyleSheet("");
        this->label_BLdown->setText(left_back);
    }
    if(msg.startsWith("@AK"))//开关量
    {
        QString bump_left("0"), bump_right("0"), bump_FL("0"),bump_FR("0"),bump_lds("0");
        int fall = msg.mid(3,2).toInt(nullptr,16);
        int bump = msg.mid(5,2).toInt(nullptr,16);
        QString dirtbox = QString::number(msg.mid(7,2).toInt(nullptr,16));
        QString carpet = QString::number(msg.mid(9,2).toInt(nullptr,16));

        QString fall_left("0");
        QString fall_right("0");
        if(fall & (1 << 1))
        {
            fall_left = "1";
            this->label_74->setStyleSheet("background:red");
        }
        else
        {
            this->label_74->setStyleSheet("");
        }
        this->label_74->setText(fall_left);

        if(fall & (1 << 0))
        {
            fall_right = "1";
            this->label_75->setStyleSheet("background:red");
        }
        else
        {
            this->label_75->setStyleSheet("");
        }
        this->label_75->setText(fall_right);


        if(bump & (1 << 0))
        {
            bump_left = "1";
            this->label_71->setStyleSheet("background:red");
        }
        else
        {
            this->label_71->setStyleSheet("");
        }
        this->label_71->setText(bump_left);

        if(bump & (1 << 1))
        {
            bump_right = "1";
            this->label_73->setStyleSheet("background:red");
        }
        else
        {
            this->label_73->setStyleSheet("");
        }
        this->label_73->setText(bump_right);

        if(bump & (1 << 5))
        {
            bump_lds = "1";
            this->label_72->setStyleSheet("background:red");
        }
        else
        {
            this->label_72->setStyleSheet("");
        }
        this->label_72->setText(bump_lds);


        if(dirtbox == "0")
            update_switch_Item("尘盒","Y");
        else
            update_switch_Item("尘盒","N");

        if(carpet == "1")
            update_switch_Item("地毯","Y");
        else
            update_switch_Item("地毯","N");

        if(Plot_Data[Bump_Plot].Enable == Channel_Enable)
            Plot_Data[Bump_Plot].value_data  = QString::number(bump);

        if(Plot_Data[Drop_Plot].Enable == Channel_Enable)
            Plot_Data[Drop_Plot].value_data  = QString::number(fall);

        TypeList.clear();
        TypeList << "开普勒" << "开普勒SE"<< "开普勒S";
        if(TypeList.contains(Deebot_Type))
        {
            if(msg.mid(11,2).toInt(nullptr,16))
            {
                update_switch_Item("边刷状态","外扩");
            }else{
                update_switch_Item("边刷状态","收回");
            }

            if(msg.mid(13,2).toInt(nullptr,16))
                update_switch_Item("接水盘","无");
            else
                update_switch_Item("接水盘","有");

            if(msg.mid(15,2).toInt(nullptr,16))
                update_switch_Item("污水安装","无");
            else
                update_switch_Item("污水安装","有");

            if(msg.mid(17,2).toInt(nullptr,16))
                update_switch_Item("污水水满","未满");
            else
                update_switch_Item("污水水满","水满");

            TypeList.clear();
            TypeList << "开普勒S";
            if(TypeList.contains(Deebot_Type))
            {
                if(msg.mid(19,2).toInt(nullptr,16))
                    update_switch_Item("滚刷抬升","触发");
                else
                    update_switch_Item("滚刷抬升","未触发");
            }
        }else{
            QString waterbox = QString::number(msg.mid(11,2).toInt(nullptr,16));

            if(waterbox == "0")
                update_switch_Item("水箱","未安装");
            if(waterbox == "1")
                update_switch_Item("水箱","普通");
            if(waterbox == "2")
                update_switch_Item("水箱","强拖");
            if(waterbox == "3")
                update_switch_Item("水箱","假强拖");
            if(waterbox == "4")
                update_switch_Item("水箱","香薰");
        }
    }
    if(msg.startsWith("@AM"))//红外全向
    {
        auto RemoteCode_SL = QString::number(msg.mid(3,2).toInt(nullptr,16));
        auto RemoteCode_L = QString::number(msg.mid(5,2).toInt(nullptr,16));
        auto RemoteCode_R = QString::number(msg.mid(7,2).toInt(nullptr,16));
        auto RemoteCode_SR = QString::number(msg.mid(9,2).toInt(nullptr,16));
        auto RemoteCode_LB = QString::number(msg.mid(11,2).toInt(nullptr,16));
        auto RemoteCode_RB = QString::number(msg.mid(13,2).toInt(nullptr,16));

        if(msg.size() > 20)
        {
            auto virtualwall_SL = QString::number(msg.mid(15,2).toInt(nullptr,16));
            auto virtualwall_L = QString::number(msg.mid(17,2).toInt(nullptr,16));
            auto virtualwall_R = QString::number(msg.mid(19,2).toInt(nullptr,16));
            auto virtualwall_SR = QString::number(msg.mid(21,2).toInt(nullptr,16));
            auto virtualwall_LB = QString::number(msg.mid(23,2).toInt(nullptr,16));
            auto virtualwall_RB = QString::number(msg.mid(25,2).toInt(nullptr,16));

            this->virtualwall_SL->setText(virtualwall_SL);
            this->virtualwall_L->setText(virtualwall_L);
            this->virtualwall_R->setText(virtualwall_R);
            this->virtualwall_SR->setText(virtualwall_SR);
            this->virtualwall_LB->setText(virtualwall_LB);
            this->virtualwall_RB->setText(virtualwall_RB);
        }
        this->label_67->setText(RemoteCode_SL);
        this->label_65->setText(RemoteCode_L);
        this->label_66->setText(RemoteCode_R);
        this->label_68->setText(RemoteCode_SR);
        this->label_69->setText(RemoteCode_LB);
        this->label_70->setText(RemoteCode_RB);
    }
    if(msg.startsWith("@AW"))//强拖
    {
        TypeList.clear();
        TypeList << "开普勒";
        if(TypeList.contains(Deebot_Type))
        {
            update_Motor_Item("滚筒电机","当前值",QString::number(msg.mid(3,2).toInt(nullptr,16)));
            update_Motor_Item("滚筒电机","设定值",QString::number(msg.mid(5,2).toInt(nullptr,16)));
            update_Motor_Item("滚筒电机","PWM(%)",QString::number(msg.mid(7,2).toInt(nullptr,16)));
            update_Motor_Item("滚筒电机","电流(mA)",QString::number(msg.mid(9,4).toInt(nullptr,16)));

            if(Plot_Data[Left_Mop_Currnet].Enable == Channel_Enable)
            {
                Plot_Data[Left_Mop_Currnet].value_data  = QString::number(msg.mid(9,4).toInt(nullptr,16));
                Plot_Data[Left_Mop_Currnet].Name_String = "滚筒电流";
            }
            Plot_Data[Left_Mop_voltage].Enable = Channel_Disenabled;
            Plot_Data[Left_Mop_pwm].Enable = Channel_Disenabled;
            Plot_Data[Right_Mop_Currnet].Enable = Channel_Disenabled;
            Plot_Data[Right_Mop_voltage].Enable = Channel_Disenabled;
            Plot_Data[Right_Mop_pwm].Enable = Channel_Disenabled;
        }else{
            TypeList.clear();
            TypeList << "开普勒SE" << "开普勒S";
            if(TypeList.contains(Deebot_Type))
            {
                if(msg.mid(3,2).toInt(nullptr,16))
                {
                    update_Motor_Item("滚筒电机","当前值","-"+QString::number(static_cast<double>(msg.mid(5,2).toInt(nullptr,16))/10));
                }else{
                    update_Motor_Item("滚筒电机","当前值","+"+QString::number(static_cast<double>(msg.mid(5,2).toInt(nullptr,16))/10));
                }
                if(msg.mid(7,2).toInt(nullptr,16))
                {
                    update_Motor_Item("滚筒电机","设定值","-"+QString::number(static_cast<double>(msg.mid(9,2).toInt(nullptr,16))/10));
                }else{
                    update_Motor_Item("滚筒电机","设定值","+"+QString::number(static_cast<double>(msg.mid(9,2).toInt(nullptr,16))/10));
                }
                update_Motor_Item("滚筒电机","PWM(%)",QString::number(msg.mid(11,2).toInt(nullptr,16)));
                update_Motor_Item("滚筒电机","电流(mA)",QString::number(msg.mid(13,4).toInt(nullptr,16)));

                if(Plot_Data[Left_Mop_Currnet].Enable == Channel_Enable)
                {
                    Plot_Data[Left_Mop_Currnet].value_data  = QString::number(msg.mid(13,4).toInt(nullptr,16));
                    Plot_Data[Left_Mop_Currnet].Name_String = "滚筒电流";
                }
                Plot_Data[Left_Mop_voltage].Enable = Channel_Disenabled;
                Plot_Data[Left_Mop_pwm].Enable = Channel_Disenabled;
                Plot_Data[Right_Mop_Currnet].Enable = Channel_Disenabled;
                Plot_Data[Right_Mop_voltage].Enable = Channel_Disenabled;
                Plot_Data[Right_Mop_pwm].Enable = Channel_Disenabled;
            }else{
                auto speed = QString::number(msg.mid(3,2).toInt(nullptr,16));
                auto current = QString::number(msg.mid(5,4).toInt(nullptr,16));
                auto pwm = QString::number(msg.mid(9,2).toInt(nullptr,16));

                if(Plot_Data[Left_Mop_Currnet].Enable == Channel_Enable)
                    Plot_Data[Left_Mop_Currnet].value_data  = current;
                if(Plot_Data[Left_Mop_voltage].Enable == Channel_Enable)
                    Plot_Data[Left_Mop_voltage].value_data  = speed;
                if(Plot_Data[Left_Mop_pwm].Enable == Channel_Enable)
                    Plot_Data[Left_Mop_pwm].value_data  = pwm;

                double data_f;
                data_f = (static_cast<double>(msg.mid(3,2).toInt(nullptr,16)))/10;
                update_Motor_Item("左拖地电机","当前值",QString::number(data_f,'f',1));
                update_Motor_Item("左拖地电机","PWM(%)",pwm);
                update_Motor_Item("左拖地电机","电流(mA)",current);

                speed = QString::number(msg.mid(11,2).toInt(nullptr,16));
                current = QString::number(msg.mid(13,4).toInt(nullptr,16));
                pwm = QString::number(msg.mid(17,2).toInt(nullptr,16));

                if(Plot_Data[Right_Mop_Currnet].Enable == Channel_Enable)
                    Plot_Data[Right_Mop_Currnet].value_data  = current;
                if(Plot_Data[Right_Mop_voltage].Enable == Channel_Enable)
                    Plot_Data[Right_Mop_voltage].value_data  = speed;
                if(Plot_Data[Right_Mop_pwm].Enable == Channel_Enable)
                    Plot_Data[Right_Mop_pwm].value_data  = pwm;

                data_f = (static_cast<double>(msg.mid(11,2).toInt(nullptr,16)))/10;
                update_Motor_Item("右拖地电机","当前值",QString::number(data_f,'f',1));
                update_Motor_Item("右拖地电机","PWM(%)",pwm);
                update_Motor_Item("右拖地电机","电流(mA)",current);
            }
        }
    }
    if(msg.startsWith("@AF"))//滚刷
    {
        if(Deebot_Type == "开普勒SE")
        {
            QString cur_voltage = QString::number(static_cast<double>(msg.mid(3,2).toInt(nullptr,16)) / 10);
            QString set_voltage = QString::number(static_cast<double>(msg.mid(5,2).toInt(nullptr,16)) / 10);
            QString pwm = QString::number(msg.mid(7,2).toInt(nullptr,16));
            QString current = QString::number(msg.mid(9,4).toInt(nullptr,16));

            if(Plot_Data[Roll_Current].Enable == Channel_Enable)
                Plot_Data[Roll_Current].value_data  = current;
            if(Plot_Data[Roll_voltage].Enable == Channel_Enable)
                Plot_Data[Roll_voltage].value_data  = set_voltage;
            if(Plot_Data[Roll_pwm].Enable == Channel_Enable)
                Plot_Data[Roll_pwm].value_data  = pwm;

            update_Motor_Item("滚刷","当前值",cur_voltage);
            update_Motor_Item("滚刷","设定值",set_voltage);
            update_Motor_Item("滚刷","PWM(%)",pwm);
            update_Motor_Item("滚刷","电流(mA)",current);
        }else{
            auto voltage = QString::number(msg.mid(3,2).toInt(nullptr,16) / 10);
            auto pwm = QString::number(msg.mid(5,2).toInt(nullptr,16));
            auto current = QString::number(msg.mid(7,4).toInt(nullptr,16));

            if(Plot_Data[Roll_Current].Enable == Channel_Enable)
                Plot_Data[Roll_Current].value_data  = current;
            if(Plot_Data[Roll_voltage].Enable == Channel_Enable)
                Plot_Data[Roll_voltage].value_data  = voltage;
            if(Plot_Data[Roll_pwm].Enable == Channel_Enable)
                Plot_Data[Roll_pwm].value_data  = pwm;

            double data_f;
            data_f = (static_cast<double>(msg.mid(3,2).toInt(nullptr,16)))/10;

            update_Motor_Item("滚刷","设定值",QString::number(data_f,'f',1));
            update_Motor_Item("滚刷","PWM(%)",pwm);
            update_Motor_Item("滚刷","电流(mA)",current);
        }
    }
    if(msg.startsWith("@AS"))//水箱
    {
        TypeList.clear();
        TypeList << "开普勒" << "开普勒SE" << "开普勒S";
        if(TypeList.contains(Deebot_Type))
        {
            if(msg.mid(3,2).toInt(nullptr,16))
            {
                update_Motor_Item("清水泵","当前值","开启");
            }else{
                update_Motor_Item("清水泵","当前值","关闭");
            }
            QString tmp;
            tmp = "开"+ QString::number(msg.mid(5,2).toInt(nullptr,16))+"S ";
            tmp += "关"+ QString::number(msg.mid(7,2).toInt(nullptr,16))+"S ";
            update_Motor_Item("清水泵","设定值",tmp);            
            update_Motor_Item("清水泵","PWM(%)",QString::number(msg.mid(9,2).toInt(nullptr,16)));
            update_Motor_Item("清水泵","电流(mA)",QString::number(msg.mid(11,4).toInt(nullptr,16)));
            double data_f;
            TypeList.clear();
            TypeList << "开普勒SE" << "开普勒S";
            if(TypeList.contains(Deebot_Type))
            {
                data_f = (static_cast<double>(msg.mid(15,2).toInt(nullptr,16)))/10;
                update_Motor_Item("污水泵","当前值",QString::number(data_f,'f',1));
                data_f = (static_cast<double>(msg.mid(17,2).toInt(nullptr,16)))/10;
                update_Motor_Item("污水泵","设定值",QString::number(data_f,'f',1));
                update_Motor_Item("污水泵","PWM(%)",QString::number(msg.mid(19,2).toInt(nullptr,16)));
            }else{
                data_f = (static_cast<double>(msg.mid(15,2).toInt(nullptr,16)))/10;
                update_Motor_Item("污水泵","设定值",QString::number(data_f,'f',1));
                update_Motor_Item("污水泵","PWM(%)",QString::number(msg.mid(17,2).toInt(nullptr,16)));
            }

        }else{
            int int_tank = msg.mid(3,2).toInt(nullptr,16);
            int int_water_flag = msg.mid(5,2).toInt(nullptr,16);
            int int_pwm = msg.mid(7,2).toInt(nullptr,16);
            int int_current = msg.mid(9,4).toInt(nullptr,16);
            QString tank_type, water_flag,pwm, current;
            if(0 == int_tank)
                tank_type = "无水箱";
            if(1 == int_tank)
                tank_type = "普通水箱";
            if(2 == int_tank)
                tank_type = "强拖水箱";
            if(3 == int_tank)
                tank_type = "假强拖";
            if(4 == int_tank)
                tank_type = "香薰";
            if(int_water_flag == 0)
                water_flag = "有水";
            else
                water_flag = "无水";

            pwm = QString::number(int_pwm);
            current = QString::number(int_current);

            update_Motor_Item("清水泵","当前值",tank_type);
            update_Motor_Item("清水泵","设定值",water_flag);
            update_Motor_Item("清水泵","PWM(%)",pwm);
            update_Motor_Item("清水泵","电流(mA)",current);
        }
    }
    if(msg.startsWith("@AO"))//电源管理
    {
        auto charge_state = QString::number(msg.mid(3,2).toInt(nullptr,16)); // 充电状态
        auto bat_voltage = QString::number(msg.mid(5,4).toInt(nullptr,16)); //电池电压
        auto charge_current = QString::number(msg.mid(9,4).toInt(nullptr,16)); //充电电流
        auto battery_capacity = QString::number(msg.mid(13,2).toInt(nullptr,16)); //电池容量
        auto battery_temp = QString::number(msg.mid(15,2).toInt(nullptr,16)); //电池温度
        auto charge_voltage = QString::number(msg.mid(17,4).toInt(nullptr,16)); //充电电压
        auto charge_stage = QString::number(msg.mid(21,2).toInt(nullptr,16)); //充电阶段
        auto ref_current = QString::number(msg.mid(23,4).toInt(nullptr,16)); //基准电流

        if(Plot_Data[Battery_percent_plot].Enable == Channel_Enable)
            Plot_Data[Battery_percent_plot].value_data  = battery_capacity;
        if(Plot_Data[Battrer_voltage_plot].Enable == Channel_Enable)
            Plot_Data[Battrer_voltage_plot].value_data  = bat_voltage;
        if(Plot_Data[Battery_charge_voltage_plot].Enable == Channel_Enable)
            Plot_Data[Battery_charge_voltage_plot].value_data  = charge_voltage;
        if(Plot_Data[Battery_current_plot].Enable == Channel_Enable)
            Plot_Data[Battery_current_plot].value_data  = charge_current;
        if(Plot_Data[Battery_temp_plot].Enable == Channel_Enable)
            Plot_Data[Battery_temp_plot].value_data  = battery_temp;
        if(Plot_Data[Battery_step_plot].Enable == Channel_Enable)
            Plot_Data[Battery_step_plot].value_data  = charge_stage;
        if(Plot_Data[Battery_sandard_current_plot].Enable == Channel_Enable)
            Plot_Data[Battery_sandard_current_plot].value_data  = ref_current;
        QString str_charge_state;
        if(charge_state == "0")
        {
            str_charge_state = tr("放电中");
            this->label_14->setStyleSheet("color:black;");
            this->label_14->setText(str_charge_state);
        }
        if(charge_state == "1")
        {
            str_charge_state = tr("充电中");
            this->label_14->setStyleSheet("color:green;");
            this->label_14->setText(str_charge_state);
        }
        if(charge_state == "2")
        {
            str_charge_state = tr("充电完成");
            this->label_14->setStyleSheet("color:green;");
            this->label_14->setText(str_charge_state);
        }
        if(charge_state == "3")
        {
            str_charge_state = tr("充电错误");
            this->label_14->setStyleSheet("color:red;");
            this->label_14->setText(str_charge_state);
        }
        this->label_3->setText(bat_voltage);
        this->label_8->setText(charge_voltage);
        this->label_9->setText(charge_current);
        this->label_13->setText(battery_temp);
        this->label_15->setText(charge_stage);
        this->label_16->setText(ref_current);
        this->progressBa_quantity->setValue(battery_capacity.toInt());
    }
    if(msg.startsWith("@AR"))//陀螺仪
    {
        QString temp = msg.mid(3,4);
        gyro_value = static_cast<int>(signed_convert(temp));
        QString gyro = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(7,4);
        QString roll = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(11,4);
        QString pitch = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(15,4);
        yaw_value = static_cast<int>(signed_convert(temp));
        QString yaw = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(19,4);
        QString acc_x = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(23,4);
        QString acc_y = QString("%1").arg((signed_convert(temp)/ 100));
        temp = msg.mid(27,4);
        QString acc_z = QString("%1").arg((signed_convert(temp)/ 100));

        if(Plot_Data[Gyro_Epson].Enable == Channel_Enable)
            Plot_Data[Gyro_Epson].value_data  = gyro;
        if(Plot_Data[Gyro_Roll].Enable == Channel_Enable)
            Plot_Data[Gyro_Roll].value_data  = roll;
        if(Plot_Data[Gyro_Pitch].Enable == Channel_Enable)
            Plot_Data[Gyro_Pitch].value_data  = pitch;
        if(Plot_Data[Gyro_Yaw].Enable == Channel_Enable)
            Plot_Data[Gyro_Yaw].value_data  =  yaw;

        if(Plot_Data[Acc_X].Enable == Channel_Enable)
            Plot_Data[Acc_X].value_data  = acc_x;
        if(Plot_Data[Acc_Y].Enable == Channel_Enable)
            Plot_Data[Acc_Y].value_data  = acc_y;
        if(Plot_Data[Acc_Z].Enable == Channel_Enable)
            Plot_Data[Acc_Z].value_data  =  acc_z;
        this->label_30->setText(gyro);
        this->label_19->setText(roll);
        this->label_20->setText(pitch);
        this->label_21->setText(yaw);
        this->label_22->setText(acc_x);
        this->label_23->setText(acc_y);
        this->label_24->setText(acc_z);
        emit Gyro_Data_Updata();
    }
    if(msg.startsWith("@Ax"))//配置
    {
        auto flag = QString::number(msg.mid(3,2).toInt(nullptr,16));
        auto ultra_flag = QString::number(msg.mid(5,2).toInt(nullptr,16));
        auto bat_cap = QString::number(msg.mid(7,4).toInt(nullptr,16));
        auto fan_type = QString::number(msg.mid(11,2).toInt(nullptr,16));
        auto bend_type = QString::number(msg.mid(13,2).toInt(nullptr,16));
        if(flag == "0")
        {
            flag = tr("未获取");
        }
        else
        {
            flag = tr("已获取");
        }
        if(ultra_flag == "0")
        {
            ultra_flag = tr("无");
        }
        else
        {
            ultra_flag = tr("有");
        }
        if(fan_type == "0")
        {
            fan_type = tr("尼德科4.0");
        }
        else
        {
            if(fan_type == "1")
            {
                fan_type = tr("凯航");
            }else{
                if(fan_type == "2")
                {
                    fan_type = tr("尼德科3.0");
                }else{
                    fan_type = tr("错误");
                }
            }

        }
        if(bend_type == "0")
        {
            bend_type = tr("蠕动");
        }
        else
        {
            bend_type = tr("时光");
        }
        config_items[0][1].setText(flag);
        config_items[1][1].setText(ultra_flag);
        config_items[2][1].setText(bat_cap);
        config_items[3][1].setText(fan_type);
        config_items[4][1].setText(bend_type);
        this->config_model->setItem(0,1,&config_items[0][1]);
        this->config_model->setItem(1,1,&config_items[1][1]);
        this->config_model->setItem(2,1,&config_items[2][1]);
        this->config_model->setItem(3,1,&config_items[3][1]);
        this->config_model->setItem(4,1,&config_items[4][1]);
    }
    if(msg.startsWith("@AL"))//抹盘检测
    {
        TypeList.clear();
        TypeList << "开普勒" << "开普勒S" << "开普勒SE";
        if(TypeList.contains(Deebot_Type))
        {
            int roll_mop_install = msg.mid(3,2).toInt(nullptr,16);
            if(roll_mop_install)
                update_switch_Item("滚筒安装","无");
            else
                update_switch_Item("滚筒安装","有");

            int exp_flg = msg.mid(5,2).toInt(nullptr,16);
            if(exp_flg)
            {
                QString str;
                str = "";
                if((exp_flg>>0)&0x01)
                    str += "外扩";
                if((exp_flg>>1)&0x01)
                    str += "收回";
                if((exp_flg>>2)&0x01)
                    str += "抬升";
                update_switch_Item("外扩检测",str);
            }else{
                update_switch_Item("外扩检测","运行");
            }
        }else{
            auto mop_state = msg.mid(3,4);

            if(mop_state == "0000")
                update_switch_Item("抹盘检测","有");
            if(mop_state == "0001")
                update_switch_Item("抹盘检测","左");
            if(mop_state == "0101")
                update_switch_Item("抹盘检测","无");
            if(mop_state == "0100")
                update_switch_Item("抹盘检测","右");

            auto mop_upstate = msg.mid(7,4);
            if(mop_upstate == "0000")
                update_switch_Item("抹盘上升","没有");
            if(mop_upstate == "0001")
                update_switch_Item("抹盘上升","右边");
            if(mop_upstate == "0100")
                update_switch_Item("抹盘上升","左边");
            if(mop_upstate == "0101")
                update_switch_Item("抹盘上升","全部");

            auto mop_outstate = msg.mid(11,4);
            if(mop_outstate == "0000")
                update_switch_Item("外扩检测","运行");
            if(mop_outstate == "0001")
                update_switch_Item("外扩检测","外扩");
            if(mop_outstate == "0100")
                update_switch_Item("外扩检测","收回");
            if(mop_outstate == "0101")
                update_switch_Item("外扩检测","错误");
        }
    }
    if(msg.startsWith("@AN"))//水箱
    {
        TypeList.clear();
        TypeList << "开普勒" << "开普勒S" << "开普勒SE";
        if(TypeList.contains(Deebot_Type))
        {
            update_switch_Item("超声波AD",QString::number(msg.mid(3,4).toInt(nullptr,16)));
            update_switch_Item("污水箱AD",QString::number(msg.mid(7,4).toInt(nullptr,16)));
            update_switch_Item("清水箱水量",QString::number(msg.mid(11,4).toInt(nullptr,16)));
            if(Deebot_Type == "开普勒SE")
            {
                QString str = "";
                switch(msg.mid(15,2).toInt(nullptr,16))
                {
                case 1:
                    str = "TDK40607";
                    break;
                case 2:
                    str = "TDK40608";
                    break;
                case 3:
                    str = "BMI323";
                    break;
                default:
                    str = "错误";
                    break;
                }
                update_switch_Item("IMU型号",str);
            }
        }
    }
    if(msg.startsWith("@AJ"))//前视缓冲
    {
        QString data[10];

        QString left_side = QString::number(msg.mid(3,4).toInt(nullptr,16));
        QString left = QString::number(msg.mid(7,4).toInt(nullptr,16));
        QString mid = QString::number(msg.mid(11,4).toInt(nullptr,16));
        QString right = QString::number(msg.mid(15,4).toInt(nullptr,16));
        QString right_side = QString::number(msg.mid(19,4).toInt(nullptr,16));

        if(Plot_Data[YanBian_F].Enable == Channel_Enable)
            Plot_Data[YanBian_F].value_data  =  left_side;
        if(Plot_Data[YanBian_B].Enable == Channel_Enable)
            Plot_Data[YanBian_B].value_data  =  right_side;
        this->label_side_buff_l->setText(left_side);
        this->label_front_buff_l->setText(left);
        this->label_front_buff_f->setText(mid);
        this->label_front_buff_r->setText(right);
        this->label_side_buff_r->setText(right_side);
    }
    if(msg.startsWith("@Aj"))//版本
    {
        auto version = msg.mid(3,msg.size() - 5);
        this->label_25->setAlignment(Qt::AlignCenter);
        this->label_25->setText(version);
        TimeToPlot();
    }

    if(msg.startsWith("@AY"))
    {
        QString current_value,pwm,current;

        TypeList.clear();
        TypeList << "开普勒"<< "开普勒S" << "开普勒SE";
        if(TypeList.contains(Deebot_Type))
        {
            update_Motor_Item("外扩电机","电流(mA)",QString::number(msg.mid(3,4).toInt(nullptr,16)));
            update_Motor_Item("外扩电机","当前值",QString::number(msg.mid(13,2).toInt(nullptr,16)));
            update_Motor_Item("外扩电机","设定值",QString::number(msg.mid(15,2).toInt(nullptr,16)));
            update_Motor_Item("外扩电机","PWM(%)",QString::number(msg.mid(17,2).toInt(nullptr,16)));
        }else{
            auto data_f = (static_cast<double>(msg.mid(3,2).toInt(nullptr,16)))/10;
            current_value = QString::number(data_f,'f',1);
            pwm = QString::number(msg.mid(5,2).toInt(nullptr,16));
            current = QString::number(msg.mid(7,4).toInt(nullptr,16));
            update_Motor_Item("外扩电机","当前值",current_value);
            update_Motor_Item("外扩电机","PWM(%)",pwm);
            update_Motor_Item("外扩电机","电流(mA)",current);
        }
    }
}


QString num2hex(int num,int len)
{
    QString temp =  QString::number(num,16);
    while(temp.size() < len)
        temp = "0" + temp;
    return temp;
}

void DataDisplay::on_pushButton_1_clicked()
{
    serialport_writedata("@Bc01*\n");
    serialport_writedata("@Af00*\n");
    serialport_writedata("@Bc00*\n");
}

void DataDisplay::on_pushButton_2_clicked()
{
    serialport_writedata("@Bc01*\n");
    serialport_writedata("@Af01*\n");
    serialport_writedata("@Bc00*\n");
}


void DataDisplay::on_pushButton_3_clicked()
{
    serialport_writedata("@Bc01*\n");
    serialport_writedata("@Af02*\n");
    serialport_writedata("@Bc00*\n");
}
void DataDisplay::on_pushButton_print_clicked()
{
    auto cycle_time = this->spinBox_print->value();
    auto str_cycle = num2hex(cycle_time, 4);
    QString send_str = "@Ad" + str_cycle + "*\n";
    serialport_writedata("@Bc01*\n");//进入设置
    serialport_writedata(send_str);
}

void DataDisplay::on_pushButton_battery_check_clicked()
{
    QString send_str = "@AwFE*\n";
    serialport_writedata("@Bc01*\n");
    serialport_writedata(send_str);
    serialport_writedata("@Bc00*\n");
}

void DataDisplay::on_pushButton_clear_config_clicked()
{
    QString send_str = "@AwFF*\n";
    serialport_writedata(send_str);
}



/**
 * @brief 定时刷新绘图区
 */
void DataDisplay::replot()
{
    /*刷新X轴坐标范围*/
    this->plot->xAxis->setRange (dataPointNumber - this->spinPoints->value(), dataPointNumber);
    this->plot->replot();
}
/**
 * @brief 向绘图区增加新的数据
 * @param newData 串口数据，这是解包后的数据，真正用于绘图的数据
 */
void DataDisplay::onNewDataArrived(QStringList newData)
{
    static int data_members = 0;
    static int channel = 0;
    static int i = 0;
    volatile bool you_shall_NOT_PASS = false;

    /* When a fast baud rate is set (921kbps was the first to starts to bug),
       this method is called multiple times (2x in the 921k tests), so a flag
       is used to throttle
       TO-DO: Separate processes, buffer data (1) and process data (2) */
    while (you_shall_NOT_PASS) {}
    you_shall_NOT_PASS = true;

    if (plotting)//正在绘图
    {
        data_members = newData.size();//获取数据的长度

        for (i = 0; i < data_members; i++)//遍历数据，解析数据
        {
            /* 第一次进入，添加所有通道 */
            while (this->plot->plottableCount() <= channel)//新的数据，通道是否比之前的多
            {
                /* 添加新的通道数据 */
                this->plot->addGraph();
                this->plot->graph()->setPen (line_colors[channels % CUSTOM_LINE_COLORS]);

                QString Plot_Channel_Name;
                Plot_Channel_Name = "";
                for (uint8_t n=0;n<Plot_data_all;n++)
                {
                    if((Plot_Data[n].Channel_num == static_cast<uint8_t>(channels))&&(Plot_Data[n].Enable == Channel_Enable))
                    {
                        Plot_Channel_Name = Plot_Data[n].Name_String;
                        n = Plot_data_all;
                    }
                }

                this->plot->graph()->setName (Plot_Channel_Name);
                if(this->plot->legend->item(channels))
                {
                    this->plot->legend->item (channels)->setTextColor (line_colors[channels % CUSTOM_LINE_COLORS]);
                }
                this->listWidget_Channels->addItem(this->plot->graph()->name());
                this->listWidget_Channels->item(channel)->setForeground(QBrush(line_colors[channels % CUSTOM_LINE_COLORS]));
                channels++;
            }

            /* [TODO] Method selection and plotting */
            /* X-Y */
            if (0)
            {

            }
            /* Rolling (v1.0.0 compatible) */
            else
            {
                /* Add data to Graph 0 */
                this->plot->graph(channel)->addData (dataPointNumber, newData[channel].toDouble());
                /* Increment data number and channel */
                channel++;
            }
        }

        /* Post-parsing */
        /* X-Y */
        if (0)
        {

        }
        /* Rolling (v1.0.0 compatible) */
        else
        {
            dataPointNumber++;
            channel = 0;
        }
    }
    you_shall_NOT_PASS = false;
}
void DataDisplay::on_pushButton_send_message_clicked()
{
    QString send_str = lineEdit_senddata->text();
    serialport_writedata(send_str);
}

void DataDisplay::on_pushButton_cleanreceive_clicked()
{
    receivedata_textBrowser->setText("");
}


void DataDisplay::on_pushButton_assign_clicked()//电机控制确认
{
    QString dirction;
    QStringList left_driver = (this->lineEdit_1->text() + ":" + QString::number(this->comboBox_1->currentIndex())).split(":");
    QStringList right_driver = (this->lineEdit_2->text() + ":" + QString::number(this->comboBox_2->currentIndex())).split(":");
    QStringList side_brush;

    serialport_writedata("@Bc01*\n");

    if(this->lineEdit_3->text() == "")
    {
        side_brush = (":" + QString::number(this->comboBox_side_dir->currentIndex())).split(":");
    }else{
        side_brush = (QString::number((this->lineEdit_3->text().toFloat())*10) + ":" + QString::number(this->comboBox_side_dir->currentIndex())).split(":");
    }

    if(this->lineEdit_5->text() != "")
    {
        QString roll_vol_str = num2hex(static_cast<int>(this->lineEdit_5->text().toFloat()*10),2);
        QString cmd = "";
        cmd= "@AE" + roll_vol_str + "*\n";
        if(Deebot_Type == "开普勒S")
        {
            if(comboBox_roll_dir->currentText() == "抬升")
            {
                cmd= "@AE" + roll_vol_str + "02*\n";
            }else{
                cmd= "@AE" + roll_vol_str + "01*\n";
            }
        }
        serialport_writedata(cmd);
    }

    QStringList dirty_water_beng_brush;
    if(this->dirty_water_beng_value->text() == "")
    {
        dirty_water_beng_brush = (":" + QString::number(this->comboBox_dirty_water_dir->currentIndex())).split(":");
    }else{
        dirty_water_beng_brush = (QString::number((this->dirty_water_beng_value->text().toFloat())*10) + ":" + QString::number(this->comboBox_dirty_water_dir->currentIndex())).split(":");
    }
    if (dirty_water_beng_brush[0].isEmpty() == false)
    {
        TypeList.clear();
        TypeList << "开普勒"<< "开普勒S" << "开普勒SE";
        if(TypeList.contains(Deebot_Type))
        {
            QString cmd = "@AQ" + num2hex(dirty_water_beng_brush[0].toInt(),2) + "*\n";
            serialport_writedata(cmd);
        }
    }
    QStringList fan_motor = (this->lineEdit_fan_speed->text() + ":" + QString::number(this->comboBox_6->currentIndex())).split(":");
    QStringList water_pump = (this->lineEdit_7->text() + ":" + this->lineEdit_8->text()).split(":");

    if (left_driver[0].isEmpty() == false)
    {
        auto speed = num2hex(left_driver[0].toInt(),2);
        QString dir;
        if (left_driver[1] == "0")
            dir = "01";
        if (left_driver[1] == "1")
            dir = "02";
        QString cmd = "@AA01" + dir + speed + "*\n";
        serialport_writedata(cmd);
    }
    if (right_driver[0].isEmpty() == false)
    {
        auto speed = num2hex(right_driver[0].toInt(),2);
        QString dir;
        if (right_driver[1] == "0")
            dir = "01";
        if (right_driver[1] == "1")
            dir = "02";
        QString cmd = "@AA02" + dir + speed + "*\n";
        qDebug()<<"right driver:"<<cmd;
        serialport_writedata(cmd);
    }
    if (side_brush[0].isEmpty() == false)
    {
        auto voltage = num2hex(side_brush[0].toInt(),2);
        QString dir;
        if (side_brush[1] == "0")
            dir = "01";
        if (side_brush[1] == "1")
            dir = "02";
        QString cmd = "@AC" + dir + voltage + "*\n";
        serialport_writedata(cmd);
    }

    if (fan_motor[0].isEmpty() == false)
    {
        QString cmd;
        TypeList.clear();
        TypeList << "开普勒" <<"开普勒S" << "开普勒SE" << "法拉第S" << "SS";
        if(TypeList.contains(Deebot_Type))
        {
            QString speed_l = num2hex(fan_motor[0].toInt()&0xFF,2);
            QString speed_h = num2hex((fan_motor[0].toInt()>>8)&0xFF,2);
            cmd = "@AG" + speed_l+speed_h + "*\n";
        }else{
            auto speed = num2hex(fan_motor[0].toInt() / 100,2);
            cmd = "@AG" + speed + "*\n";
        }
        serialport_writedata(cmd);
    }
    if(water_pump[0].isEmpty() == false && water_pump[1].isEmpty() == false)
    {
        auto start_time = num2hex(water_pump[0].toInt(),2);
        auto end_time = num2hex(water_pump[1].toInt(),2);
        QString cmd = "@As" + start_time + end_time + "*\n";
        serialport_writedata(cmd);
    }

    if(this->lineEdit_mop_speed->text() != "")
    {
        QString mop_value = num2hex(lineEdit_mop_speed->text().toInt()/10,2);

        TypeList.clear();
        TypeList << "SS" << "开普勒SE"<< "开普勒S";
        if(TypeList.contains(Deebot_Type))
        {
            mop_value = num2hex((QString::number((lineEdit_mop_speed->text().toFloat())*10)).toInt(),2);
        }

        QString dirction;
        if(comboBox_mop_dir->currentText() == "正转")
            dirction = "01";
        else
            dirction = "02";
        QString cmd = "@AV" + dirction + mop_value + "*\n";
        serialport_writedata(cmd);
    }

    QString value;
    QString cmd;
    cmd = "";

    if(this->comboBox_mop_out->currentText()=="外扩")
    {
        if(this->lineEdit_mop_out_value->text() != "")
        {
            value = num2hex(static_cast<int>(this->lineEdit_mop_out_value->text().toFloat()),2);
            cmd = "@AX00"+ value+ "*\n";
            serialport_writedata(cmd);
        }
    }else{//抬升
        cmd = "@AX0500*\n";
        serialport_writedata(cmd);
    }

}

void DataDisplay::on_pushButton_4_clicked()//电机控制，关闭
{
    lineEdit_1->setText("0");
    lineEdit_2->setText("0");
    lineEdit_3->setText("0");
    lineEdit_5->setText("0");
    lineEdit_fan_speed->setText("0");
    lineEdit_7->setText("0");
    lineEdit_8->setText("0");
    lineEdit_mop_speed->setText("0");
    dirty_water_beng_value->setText("0");
    lineEdit_mop_out_value->setText("0");
    serialport_writedata("@Bc00*\n");
}
/**
 * @brief 自动缩放Y轴
 */
void DataDisplay::on_pushButton_AutoScale_clicked()
{
    this->plot->yAxis->rescale(true);
    this->spinAxesMax->setValue(int(this->plot->yAxis->range().upper) + int(this->plot->yAxis->range().upper*0.1));
    this->spinAxesMin->setValue(int(this->plot->yAxis->range().lower) + int(this->plot->yAxis->range().lower*0.1));
}
/**
 * @brief 显示所有通道
 */
void DataDisplay::on_pushButton_ResetVisible_clicked()
{
    for(int i=0; i<this->plot->graphCount(); i++)
    {
        this->plot->graph(i)->setVisible(true);
        this->listWidget_Channels->item(i)->setBackground(Qt::NoBrush);
    }
}
/**
 * @brief 保存PNG图片
 */
void DataDisplay::on_savePNGButton_clicked()
{
    this->plot->savePng (QString::number(dataPointNumber) + ".png", 1920, 1080, 2, 50);
}
/**
 * @brief 当Y轴最小值变化时，也就是spinAxesMin控件变化
 * @param arg1
 */
void DataDisplay::on_spinAxesMin_valueChanged(int arg1)
{
    this->plot->yAxis->setRangeLower (arg1);
    this->plot->replot();
}
/**
 * @brief 当Y轴最大值变化时，也就是spinAxesMax控件变化
 * @param arg1
 */
void DataDisplay::on_spinAxesMax_valueChanged(int arg1)
{
    this->plot->yAxis->setRangeUpper (arg1);
    this->plot->replot();
}
/**
 * @brief 当Y轴单元格长度变化，也就是spinYStep控件发生变化
 * @param arg1
 */
void DataDisplay::on_spinYStep_valueChanged(int arg1)
{
    this->plot->yAxis->ticker()->setTickCount(arg1);
    this->plot->replot();
    this->spinYStep->setValue(this->plot->yAxis->ticker()->tickCount());
}
/**
 * @brief 打印鼠标在绘图区中的坐标
 * @param event
 */
void DataDisplay::onMouseMoveInPlot(QMouseEvent *event)
{
    int xx = int(this->plot->xAxis->pixelToCoord(event->x()));
    int yy = int(this->plot->yAxis->pixelToCoord(event->y()));
    QString coordinates("X: %1 Y: %2");
    coordinates = coordinates.arg(xx).arg(yy);
//    ui->statusBar->showMessage(coordinates);
}
/**
 * @brief 鼠标滚轮在绘图区滚动
 * @param event
 */
void DataDisplay::on_mouse_wheel_in_plot (QWheelEvent *event)
{
    QWheelEvent inverted_event = QWheelEvent(event->posF(), event->globalPosF(),
                                             -event->pixelDelta(), -event->angleDelta(),
                                             0, Qt::Vertical, event->buttons(), event->modifiers());
    QApplication::sendEvent (this->spinPoints, &inverted_event);
}


/**
 * @brief 鼠标选择图例或绘图区中曲线
 * @param plottable
 * @param event
 */
void DataDisplay::channel_selection (void)
{
    /* 同步用于选择图例和选择曲线 */
    for (int i = 0; i < this->plot->graphCount(); i++)
    {
        QCPGraph *graph = this->plot->graph(i);
        QCPPlottableLegendItem *item = this->plot->legend->itemWithPlottable(graph);

        if (item->selected() || graph->selected())
        {
            item->setSelected(true);

            QPen pen;
            pen.setWidth(3);
            pen.setColor(line_colors[14]);
            graph->selectionDecorator()->setPen(pen);

            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}


/**
 * @brief 双击图例时，用于给图例修改名字
 * @param legend
 * @param item
 */
void DataDisplay::legend_double_click(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)
{
    Q_UNUSED (legend)
    Q_UNUSED(event)
    if (item)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText (this, "Change channel name", "New name:", QLineEdit::Normal, plItem->plottable()->name(), &ok, Qt::Widget);//最有一个参数是Qt::Popup，无法输入中文
        if (ok)
        {
            plItem->plottable()->setName(newName);
            for(int i=0; i<this->plot->graphCount(); i++)
            {
                this->listWidget_Channels->item(i)->setText(this->plot->graph(i)->name());
            }
            this->plot->replot();
        }
    }
}

/**
 * @brief 当X轴单元格宽度变化时，也就是spinPoints控件发生变化
 * @param arg1
 */
void DataDisplay::on_spinPoints_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    this->plot->xAxis->setRange (dataPointNumber - this->spinPoints->value(), dataPointNumber);
    this->plot->replot();
}
/**
 * @brief 使用当前日期/时间戳创建新的 CSV 文件
 *
 */
void DataDisplay::openCsvFile(void)
{
    save_file_counter = 0;
    m_csvFile = new QFile(QDateTime::currentDateTime().toString("yyyy-MM-d-HH-mm-ss-")+"data-out.csv");//创建文件
    if(!m_csvFile)
        return;
    if (!m_csvFile->open(QIODevice::ReadWrite | QIODevice::Text))
        return;
}
/**
 * @brief 关闭CSV文件
 *
 */
void DataDisplay::closeCsvFile(void)
{
    if(!m_csvFile) return;
    m_csvFile->close();
    if(m_csvFile) delete m_csvFile;
    m_csvFile = nullptr;
}

/**
 * @brief 保存接收到数据
 *
 */
void DataDisplay::saveStream(QStringList newData)
{
    save_file_counter++;

    // 获取当前时间
    QTime current_time = QTime::currentTime();
//    qDebug() << "Local Time:" << current_time;

    // 获取当前日期
    QDate current_date = QDate::currentDate();

    // 结合当前时间和当前日期创建一个 QDateTime 对象
    QDateTime local_date_time(current_date, current_time);

    // 将 QDateTime 对象转换为 UTC 时间
    QDateTime utc_date_time = local_date_time.toUTC();

    // 将 QDateTime 对象转换为32位无符号的UTC时间戳
    uint32_t utc_timestamp = static_cast<uint32_t>(utc_date_time.toTime_t());
//    qDebug() << "UTC Timestamp:" << utc_timestamp;

    if(save_file_counter == 5)
    {
        if(!m_csvFile)
            return;
        if(this->radioButton_RecordData->isChecked())
        {
            QTextStream out(m_csvFile);
            QString time = "时间";
            out << time << ",";
            out << "time" << ",";
            for(uint8_t i=0;i<All_Channel_num;i++)
            {
                QString Plot_Channel_Name;
                Plot_Channel_Name = "";
                for (uint8_t n=0;n<Plot_data_all;n++)
                {
                    if((Plot_Data[n].Channel_num == i)&&(Plot_Data[n].Enable == Channel_Enable))
                    {
                        Plot_Channel_Name = Plot_Data[n].Name_String;
                        n = Plot_data_all;
                    }

                }
                out << Plot_Channel_Name << ",";
            }
            out << "\n";
        }
        last_updata_time =sys_tick_100ms;
    }
    if(save_file_counter > 5)
    {
        plotting = true;
        if(!m_csvFile)
            return;
        if(this->radioButton_RecordData->isChecked())
        {
            QTextStream out(m_csvFile);
            current_time = QTime::currentTime();
            QString now_time = current_time.toString()+":"+QString::number(QTime::currentTime().msec(),10);
            out << now_time << ",";
//            double data_time = ((static_cast<double>(sys_tick_100ms) -static_cast<double>(last_updata_time))/10);
            QString data_time_t = QString::number(utc_timestamp-start_utc_time);
            out << data_time_t << ",";
            foreach (const QString &str, newData) {
                out << str << ",";
            }
            out << "\n";
        }
    }else{
        start_utc_time =utc_timestamp;
    }

}
/**
 * @brief 双击通道列表，用于关闭某个通道显示
 * @param item
 */
void DataDisplay::on_listWidget_Channels_itemDoubleClicked(QListWidgetItem *item)
{
    int graphIdx = this->listWidget_Channels->currentRow();

    if(this->plot->graph(graphIdx)->visible())
    {
        this->plot->graph(graphIdx)->setVisible(false);
        item->setBackgroundColor(Qt::black);
    }
    else
    {
        this->plot->graph(graphIdx)->setVisible(true);
        item->setBackground(Qt::NoBrush);
    }
    this->plot->replot();
}
/**
 * @brief 是否显示串口发送模块
 */
void DataDisplay::on_pushButton_showsend_clicked()
{
    //if(this->serialport->isOpen())
    {
        if(this->pushButton_showsend->text() == "打开发送")
        {
            /*隐藏发送*/
            this->groupBox_senddata->setVisible(true);
            this->pushButton_showsend->setText("隐藏发送");
        }
        else if(this->pushButton_showsend->text() == "隐藏发送")
        {
            /*打开发送*/
            this->receivedata_textBrowser->clear();
            this->groupBox_senddata->setVisible(false);
            this->radioButton_enablereceive->setChecked(false);
            this->pushButton_showsend->setText("打开发送");
        }
    }
}

void DataDisplay::on_pushButton_PausePlot_clicked()
{
    if (plotting)
    {
        updateTimer.stop();
        plotting = false;
        this->pushButton_PausePlot->setText("开始绘制");
    }else{
        updateTimer.start();
        plotting = true;
        this->pushButton_PausePlot->setText("暂停绘制");
    }
}
/**
 * @brief 读取存储在机器内部的电量信息
 */
void DataDisplay::on_pushButton_readbatterinf_clicked()
{
    if(widget_serial_port->isopen_status)
    {
        serialport_writedata("@BP*");
        this->radioButton_enablereceive->setChecked(true);
        if(this->pushButton_showsend->text() == "打开发送")
        {
            /*隐藏发送*/
            this->groupBox_senddata->setVisible(true);
            this->pushButton_showsend->setText("隐藏发送");
        }
    }
}
/**
 * @brief 设置底板时间
 */
void DataDisplay::on_pushButton_set_time_clicked()
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString currentTime_set;
    currentTime_set = "";

    currentTime_set += QString("%1").arg(currentTime.mid(0,4).toInt(nullptr,10),4,16,QLatin1Char('0'));//年
    currentTime_set += QString("%1").arg(currentTime.mid(4,2).toInt(nullptr,10),2,16,QLatin1Char('0'));//月
    currentTime_set += QString("%1").arg(currentTime.mid(6,2).toInt(nullptr,10),2,16,QLatin1Char('0'));//日
    currentTime_set += QString("%1").arg(currentTime.mid(8,2).toInt(nullptr,10),2,16,QLatin1Char('0'));//小时
    currentTime_set += QString("%1").arg(currentTime.mid(10,2).toInt(nullptr,10),2,16,QLatin1Char('0'));//分
    currentTime_set += QString("%1").arg(currentTime.mid(12,2).toInt(nullptr,10),2,16,QLatin1Char('0'));//秒

    serialport_writedata("@ST"+currentTime_set+"*");
}

void DataDisplay::on_pushButton_enable_printf_clicked()
{
    if(widget_serial_port->isopen_status)
    {
        serialport_writedata("@DB*");
        this->radioButton_enablereceive->setChecked(true);
        if(this->pushButton_showsend->text() == "打开发送")
        {
            /*隐藏发送*/
            this->groupBox_senddata->setVisible(true);
            this->pushButton_showsend->setText("隐藏发送");
        }
    }
}

void DataDisplay::on_pushButton_hide_all_clicked()
{
    for(int i=0; i<this->plot->graphCount(); i++)
    {
        this->plot->graph(i)->setVisible(false);
        this->listWidget_Channels->item(i)->setBackground(Qt::black);
    }
}

/* 修改型号 */
void DataDisplay::on_comboBox_deebot_type_currentIndexChanged(const QString &arg1)
{
    Deebot_Type = arg1;
    switch_init(Deebot_Type);
    MotorModelInit();

    /************************************/
    this->label_dirty_water->setVisible(false);
    this->comboBox_dirty_water_dir->setVisible(false);
    this->dirty_water_beng_value->setVisible(false);
    this->label_dirty_water_unit->setVisible(false);

    this->label_mop_type->setText("拖地转速");
    this->label_36->setText(" rpm");
    this->label_72->setVisible(true);
    this->comboBox_mop_out->setEnabled(false);


    if(Deebot_Type == "开普勒")
    {
        this->label_dirty_water->setVisible(true);
        this->comboBox_dirty_water_dir->setVisible(true);
        this->dirty_water_beng_value->setVisible(true);
        this->label_dirty_water_unit->setVisible(true);
        this->comboBox_mop_out->setEnabled(true);
    }

    TypeList.clear();
    TypeList << "SS" << "开普勒SE";
    if(TypeList.contains(Deebot_Type))
    {
        this->comboBox_side_dir->setEnabled(false);
    }else{
        this->comboBox_side_dir->setEnabled(true);
    }

    TypeList.clear();
    TypeList << "SS" << "开普勒SE"<< "开普勒S";
    if(TypeList.contains(Deebot_Type))
    {
        this->label_dirty_water->setVisible(true);
        this->comboBox_dirty_water_dir->setVisible(true);
        this->dirty_water_beng_value->setVisible(true);
        this->label_dirty_water_unit->setVisible(true);
        this->comboBox_mop_out->setEnabled(true);

        this->label_mop_type->setText("拖地电压");
        this->label_36->setText(" V");
    }
    TypeList.clear();
    TypeList << "开普勒S";
    if(TypeList.contains(Deebot_Type))
    {
        this->comboBox_roll_dir->setEnabled(true);
    }else{
        this->comboBox_roll_dir->setEnabled(false);
    }
}

void DataDisplay::on_pushButton_clear_data_clicked()
{
    lineEdit_1->setText("");
    lineEdit_2->setText("");
    lineEdit_3->setText("");
    lineEdit_5->setText("");
    lineEdit_fan_speed->setText("");
    lineEdit_7->setText("");
    lineEdit_8->setText("");
    lineEdit_mop_speed->setText("");
    lineEdit_mop_out_value->setText("");
    dirty_water_beng_value->setText("");
}

void DataDisplay::serialport_writedata(QString data)
{
    widget_serial_port->writedata(data.toLocal8Bit());
}
