#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include "ui_datadisplay.h"
#include <math.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <string>
#include <QStandardItemModel>
#include <QTimer>
#include <QTime>
#include <QQueue>
#include "qcustomplot.h"
#include "serialcom.h"
#include <vector>



using std::string;
extern QRegExp reg;
extern std::map<string,string> serial_config;

#define START_MSG       '@'
#define END_MSG         '*'

#define WAIT_START      1
#define IN_MESSAGE      2
#define UNDEFINED       3

#define CUSTOM_LINE_COLORS   45
#define GCP_CUSTOM_LINE_COLORS 4

class DataDisplay : public QWidget, private Ui::DataDisplay
{
    Q_OBJECT

public:
    explicit DataDisplay(QWidget *parent = nullptr);

    QTimer *timer_100ms;
    bool is_work;
    void protocol_parser(QString &msg);
    QString byte_info;
    QTime current_time;
    uint32_t  start_utc_time;
    QSet<QString> getAllMatchResults(QString & text ,const QRegExp &regexp);
    void MotorModelInit();
    void config_init();
    void switch_init(QString dev_type);
    time_t getDateFromMacro(char const *time) ;
    double signed_convert(QString &str);
    QStandardItemModel * motor_model;
    QStandardItemModel * config_model;
    QStandardItemModel * switch_model;
    QStandardItem motor_items[11][5];
    QStandardItem config_items[5][2];
    void customPlotinit();
    double yAxis_lower, yAxis_upper;
    bool _start;
    bool _pause;
    int gyro_value;
    int yaw_value;
    int STATE;
    QString Deebot_Type;
    QStringList TypeList;

private:
    /* Line colors */
    QColor line_colors[CUSTOM_LINE_COLORS];
    QColor gui_colors[GCP_CUSTOM_LINE_COLORS];

    /* Main info */
    bool connected;                                                                       // 串口打开状态变量
    bool plotting;                                                                        // 是否在绘制状态，
    int dataPointNumber;                                                                  // 记录总共接收了多少次数据
    /* Channels of data (number of graphs) */
    int channels;

    /* Data format */
    int data_format;

    /* Textbox Related */
    bool filterDisplayedData = true;

    /* Listview Related */
    QStringListModel *channelListModel;
    QStringList     channelStrList;

    //-- CSV file to save data
    QFile* m_csvFile = nullptr;

    uint32_t save_file_counter;

    QTimer updateTimer;
    enum data_plot
    {
        Left_Driver_Current,//0
        Left_Driver_current_speed,//1
        Left_Driver_target_speed,//2
        Left_Driver_PWM,//3
        Right_Driver_Current,//4
        Right_Driver_current_speed,//5
        Right_Driver_target_speed,//6
        Right_Driver_PWM,//7
        Left_Brust_Current,//8
        Left_Brust_voltage,//9
        Left_Brust_pwm,//10
        Right_Brust_Current,//11
        Right_Brust_voltage,//12
        Right_Brust_pwm,//13
        Roll_Current,//14
        Roll_voltage,//15
        Roll_pwm,//16
        Left_Mop_Currnet,//17
        Left_Mop_voltage,//18
        Left_Mop_pwm,//19
        Right_Mop_Currnet,//20
        Right_Mop_voltage,//21
        Right_Mop_pwm,//22
        Gyro_Epson,//23
        Gyro_Roll,//24
        Gyro_Pitch,//25
        Gyro_Yaw,//26
        Epson_Yaw,//27
        Acc_X,//28
        Acc_Y,//29
        Acc_Z,//30
        YanBian_F,//29 前沿边
        YanBian_B,//30 后沿边
        Bump_Plot,//31
        Drop_Plot,//32
        Ldown_Plot,//33
        FLdown_Plot,//34
        FRdown_Plot,//35
        Rdown_Plot,//36
        BR_down_Plot,//37
        BL_down_Plot,//38
        Battery_percent_plot,//39
        Battrer_voltage_plot,//40
        Battery_charge_voltage_plot,//41
        Battery_current_plot,//42
        Battery_temp_plot,//43
        Battery_step_plot,//44
        Battery_sandard_current_plot,//45
        Plot_data_all
    };
    uint8_t All_Channel_num;
    uint32_t sys_tick_100ms;
    uint32_t last_updata_time;

    typedef enum channel_s{
        Channel_Enable,
        Channel_Disenabled
    }channel_enum;

    typedef struct Plot_S{
        channel_enum Enable;//是否使能
        uint32_t receive_ok;//是否收到数据
        uint32_t Channel_num;//通道ID
        QString Name_String;//通道名字
        QString value_data;//数据
    }Plot_Struct;
    Plot_Struct Plot_Data[Plot_data_all];

    uint32_t Sys;

    void openCsvFile(void);
    void closeCsvFile(void);
    uint8_t Deal_Chanel_Value();
    void TimeToPlot();
    void saveStream(QStringList newData);
    void update_switch_Item(QString name,QString str);
    void update_Motor_Item(QString row_name,QString col_name,QString str);
    void serialport_writedata(QString data);

signals:
    void Motor_contral_stop();
    void Gyro_Data_Updata();
    void newData(QStringList data);
private slots:
    void on_pushButton_start_clicked();
    void serial_receive_data(QByteArray data);
    void SerialComStatusChange(bool status);
    void timer_100ms_dealwith();
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_print_clicked();
    void on_pushButton_battery_check_clicked();
    void on_pushButton_clear_config_clicked();
    void on_pushButton_send_message_clicked();
    void on_pushButton_cleanreceive_clicked();
    void on_pushButton_assign_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_AutoScale_clicked();
    void on_pushButton_ResetVisible_clicked();
    void on_savePNGButton_clicked();
    void on_spinAxesMin_valueChanged(int arg1);
    void on_spinAxesMax_valueChanged(int arg1);
    void on_spinYStep_valueChanged(int arg1);
    void on_spinPoints_valueChanged(int arg1);
    void on_listWidget_Channels_itemDoubleClicked(QListWidgetItem *item);
    void on_mouse_wheel_in_plot (QWheelEvent *event);
    void onMouseMoveInPlot(QMouseEvent *event);
    void channel_selection (void);
    void legend_double_click(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
    void replot();
    void onNewDataArrived(QStringList newData);
    void on_pushButton_showsend_clicked();
    void on_pushButton_PausePlot_clicked();
    void on_pushButton_readbatterinf_clicked();
    void on_pushButton_set_time_clicked();
    void on_pushButton_enable_printf_clicked();
    void on_pushButton_hide_all_clicked();
    void on_comboBox_deebot_type_currentIndexChanged(const QString &arg1);
    void on_pushButton_clear_data_clicked();
};

#endif // DATADISPLAY_H
