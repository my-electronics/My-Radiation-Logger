#include "serialportdialog.h"
#include "ui_serialportdialog.h"

#include <QDateTime>
#include <QMessageBox>

SerialPortDialog::SerialPortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortDialog)
{
    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->portNameComboBox->addItem(info.portName());
    }

    ui->baudRateComboBox->addItem("9600", QSerialPort::Baud9600);
    ui->baudRateComboBox->addItem("19200", QSerialPort::Baud19200);
    ui->baudRateComboBox->addItem("38400", QSerialPort::Baud38400);
    ui->baudRateComboBox->addItem("115200", QSerialPort::Baud115200);

    ui->dataBitsComboBox->addItem("5", QSerialPort::Data5);
    ui->dataBitsComboBox->addItem("6", QSerialPort::Data6);
    ui->dataBitsComboBox->addItem("7", QSerialPort::Data7);
    ui->dataBitsComboBox->addItem("8", QSerialPort::Data8);

    ui->parityComboBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityComboBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityComboBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityComboBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityComboBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsComboBox->addItem("1", QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->stopBitsComboBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    #endif
    ui->stopBitsComboBox->addItem("2", QSerialPort::TwoStop);

    ui->flowControlComboBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlComboBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlComboBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this,  &SerialPortDialog::accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this,  &SerialPortDialog::rejected);
}


void SerialPortDialog::open()
{
    ui->portNameComboBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->portNameComboBox->addItem(info.portName());
    }
    if (settings.portNameIndex < ui->portNameComboBox->count())
        ui->portNameComboBox->setCurrentIndex(settings.portNameIndex);

    QDialog::open();
}

SerialPortDialog::Settings SerialPortDialog::getSettings(void)
{
    return settings;
}

QString SerialPortDialog::getSettingsString(void)
{
    QString settingsString = ui->portNameComboBox->currentText() + ": "
                           + ui->baudRateComboBox->currentText() + " "
                           + ui->dataBitsComboBox->currentText()
                           + ui->parityComboBox->currentText()[0]
                           + ui->stopBitsComboBox->currentText()[0];
    return settingsString;
}

void SerialPortDialog::setSettings(Settings s)
{
    settings = s;
    updateSettings();
}

void SerialPortDialog::updateSettings(void)
{
    if (settings.portNameIndex < ui->portNameComboBox->count())
        ui->portNameComboBox->setCurrentIndex(settings.portNameIndex);
    ui->baudRateComboBox->setCurrentIndex(settings.baudRateIndex);
    ui->dataBitsComboBox->setCurrentIndex(settings.dataBitsIndex);
    ui->parityComboBox->setCurrentIndex(settings.parityIndex);
    ui->stopBitsComboBox->setCurrentIndex(settings.stopBitsIndex);
    ui->flowControlComboBox->setCurrentIndex(settings.flowControlIndex);
}

void SerialPortDialog::accepted()
{
    serial->setPortName(ui->portNameComboBox->currentText());
    serial->setBaudRate(QSerialPort::BaudRate(ui->baudRateComboBox->itemData(ui->baudRateComboBox->currentIndex()).toInt()));
    serial->setDataBits(QSerialPort::DataBits(ui->dataBitsComboBox->itemData(ui->dataBitsComboBox->currentIndex()).toInt()));
    serial->setParity(QSerialPort::Parity(ui->parityComboBox->itemData(ui->parityComboBox->currentIndex()).toInt()));
    serial->setStopBits(QSerialPort::StopBits(ui->stopBitsComboBox->itemData(ui->stopBitsComboBox->currentIndex()).toInt()));
    serial->setFlowControl(QSerialPort::FlowControl(ui->flowControlComboBox->itemData(ui->flowControlComboBox->currentIndex()).toInt()));

    settings.portNameIndex = ui->portNameComboBox->currentIndex();
    settings.baudRateIndex = ui->baudRateComboBox->currentIndex();
    settings.dataBitsIndex = ui->dataBitsComboBox->currentIndex();
    settings.parityIndex = ui->parityComboBox->currentIndex();
    settings.stopBitsIndex = ui->stopBitsComboBox->currentIndex();
    settings.flowControlIndex = ui->flowControlComboBox->currentIndex();
}


void SerialPortDialog::rejected()
{
    updateSettings();
}


SerialPortDialog::~SerialPortDialog()
{
    delete ui;
}
