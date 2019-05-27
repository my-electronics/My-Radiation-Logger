#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class SerialPortDialog;
}

class SerialPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortDialog(QWidget *parent = nullptr);
    ~SerialPortDialog();

    struct Settings {
        int portNameIndex;
        //QString portNameString;
        int baudRateIndex;
        //QString baudRateString;
        int dataBitsIndex;
        int parityIndex;
        int stopBitsIndex;
        int flowControlIndex;
    };

    Settings settings;

    QSerialPort *serial = nullptr;
    void open();
    Settings getSettings(void);
    QString getSettingsString(void);
    void setSettings(Settings);

private slots:
    void accepted();
    void rejected();

private:
    Ui::SerialPortDialog *ui;

    void updateSettings(void);
};

#endif // SETTINGSDIALOG_H
