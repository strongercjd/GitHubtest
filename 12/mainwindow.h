#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datadisplay.h>
#include <QWidget>
#include <Windows.h>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>
#include <stdio.h>
#include <fstream>
#include <QProcess>
#include <QFile>


using std::string;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


extern std::map<string,string> serial_config;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString account;
    QString key;
    DataDisplay *p_data_display;
    QString get_SerialNumber();
    QString hash_Encryption(const QString temp);
    QString format_HASHString(const QString hashtemp);
    QString remove_Format(const QString formathash);
    bool check_key_existence(QString & key);
    void save_key(QString & key);

private slots:
    void on_pushButton_register_clicked();
    void on_pushButton_login_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
