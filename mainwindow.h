#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QSettings>
#include <QtSerialPort/QSerialPort>

#include "serialportdialog.h"
#include "geigertubedialog.h"

namespace Ui {
class MainWindow;
}

//class SerialPortDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void start();
    void clear();
    void readData();
    void openSerialPortDialog();
    void serialPortDialogFinished(int);
    void fileOpen();
    void fileSaveAs();
    void about();

private:
    Ui::MainWindow *ui; 

    bool started;
    QSerialPort *serial;

    SerialPortDialog *serialPortDialog;
    GeigerTubeDialog *geigerTubeDialog;


    //QSettings = settings;
    QLabel *statusLabel;
    QLabel *serialLabel;
    int time;
    QVector<double> X, Y;
    void setEnabled(bool);
    QSettings *settings;

};

#endif // MAINWINDOW_H
