#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString key;
    MainWindow w;
    auto word = w.check_key_existence(key);
    if(word)
    {
        qDebug()<<"the key exists ";
        auto realpassword = w.remove_Format(w.format_HASHString(w.hash_Encryption(w.get_SerialNumber())));
        if(key == realpassword)
        {
            w.p_data_display = new DataDisplay();
            w.p_data_display->show();
        }else{
            qDebug()<<"the key is error ";
            w.show();
        }
    }
    else
    {
        qDebug()<<"the key does not exist ";
        w.show();
    }

    return a.exec();
}
