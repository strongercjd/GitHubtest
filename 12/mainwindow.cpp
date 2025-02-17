#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <map>
#include <string>


using std::map;
using std::string;
std::map<string,string> serial_config;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    p_data_display = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::check_key_existence(QString & key)
{
    std::ifstream file;
    QString file_name;
//    file_name = QCoreApplication::applicationDirPath() + "key.txt";
    file_name = "./key.txt";
    file.open(file_name.toStdString());
    std::string std_key;
    if(file.is_open())
    {
        file>>std_key;
        key = QString::fromStdString(std_key);
        return true;
    }
    else
    {
        return false;
    }
}
/*
 * 生成序列号
 */
QString MainWindow::get_SerialNumber()
{
    QString cpu_id = "";
    QProcess p(0);
    p.start("wmic CPU get ProcessorID");
    p.waitForStarted();
    p.waitForFinished();
    cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
    cpu_id = cpu_id.remove("ProcessorId").trimmed();

    QString lpRootPathName = "C:\\";
    LPTSTR lpVolumeNameBuffer=new TCHAR[12];//磁盘卷标
    DWORD VolumeSerialNumber;//硬盘序
    DWORD nVolumeNameSize=12;// 卷标的字符串长度列号
    DWORD MaximumComponentLength;// 最大的文件长度
    LPTSTR lpFileSystemNameBuffer=new TCHAR[10];// 存储所在盘符的分区类型的长指针变量
    DWORD nFileSystemNameSize=10;// 分区类型的长指针变量所指向的字符串长度
    DWORD FileSystemFlags;// 文件系统的一此标志
    GetVolumeInformation((LPTSTR)lpRootPathName.utf16(),
                         lpVolumeNameBuffer, nVolumeNameSize,
                         &VolumeSerialNumber, &MaximumComponentLength,
                         &FileSystemFlags,
                         lpFileSystemNameBuffer, nFileSystemNameSize);
    return (cpu_id.mid(0,4) + "D-"+ cpu_id.mid(4,4) + "R-" +
            cpu_id.mid(8,4) + "E-" + cpu_id.mid(12,4) + "A-" +
            QString::number(VolumeSerialNumber,10).mid(0,4)+"M");
}
/*
 * MD5加密
 */
QString MainWindow::hash_Encryption(const QString temp)
{
    QByteArray byte_array;
    byte_array.append(temp);
    QByteArray hash_byte_array = QCryptographicHash::hash(byte_array,QCryptographicHash::Md5);
    return hash_byte_array.toHex().toUpper();
}
/*
 * 格式化生成序列号
 */
QString MainWindow::format_HASHString(const QString hashtemp)
{
    QString retemp = "";
    for(int i = 0; i < 7; i++)
    {
        retemp += hashtemp.mid(4*i,4) + "-";
    }
    retemp += hashtemp.mid(28,4);
    return retemp;
}
/*
 * 去除格式，还原MD5序列
 */
QString MainWindow::remove_Format(const QString formathash)
{
    QString temp = "";
    for(int i = 0; i < 8; i++)
    {
        temp += formathash.mid(5*i,4);
    }
    return temp;
}


void MainWindow::on_pushButton_register_clicked()
{
    ui->lineEdit_account->setText(get_SerialNumber());
}
void MainWindow::save_key(QString & key)
{
    QString file_name;
//    file_name = QCoreApplication::applicationDirPath() + "/key.txt";
    file_name = "./key.txt";
    FILE *pfile = fopen(file_name.toLatin1().data(),"w");
    std::string stdstr = key.toStdString();
    if(pfile)
    {
        fprintf(pfile,"%s",stdstr.c_str());
        fclose(pfile);
    }
}
void MainWindow::on_pushButton_login_clicked()
{
    auto password = ui->lineEdit_password->text();
    auto realpassword = remove_Format(format_HASHString(hash_Encryption(get_SerialNumber())));
    if(password == realpassword)
    {
        save_key(password);
        this->close();
        p_data_display = new DataDisplay();
        p_data_display->show();
    }
}
