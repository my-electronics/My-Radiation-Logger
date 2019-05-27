#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    started = false;

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    serialPortDialog = new SerialPortDialog(this);
    serialPortDialog->setWindowFlags(serialPortDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    geigerTubeDialog = new GeigerTubeDialog(this);
    geigerTubeDialog->setWindowFlags(geigerTubeDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QSettings settings("Settings.ini", QSettings::IniFormat); // QSettings::NativeFormat
    try {
        SerialPortDialog::Settings s1;
        s1.portNameIndex = settings.value("PortName", "0").toInt();
        s1.baudRateIndex = settings.value("BaudRate", "0").toInt();
        s1.dataBitsIndex = settings.value("DataBits", "3").toInt();
        s1.parityIndex = settings.value("Parity", "0").toInt();
        s1.stopBitsIndex = settings.value("StopBits", "0").toInt();
        s1.flowControlIndex = settings.value("FlowControl", "0").toInt();
        serialPortDialog->setSettings(s1);

        GeigerTubeDialog::Settings s2;
        s2.conversionFactor = settings.value("ConversionFactor", "0.057").toDouble();
        geigerTubeDialog->setSettings(s2);
    }
    catch (...) {
        settings.remove("");
    }

    connect(ui->actionSerialPort, &QAction::triggered, this, &MainWindow::openSerialPortDialog);
    connect(ui->actionGeigerTube, &QAction::triggered, geigerTubeDialog, &GeigerTubeDialog::open);
    connect(serialPortDialog, SIGNAL(finished (int)), this, SLOT(serialPortDialogFinished(int)));
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::fileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->buttonStart, &QPushButton::clicked, this,  &MainWindow::start);
    connect(ui->buttonClear, &QPushButton::clicked, this,  &MainWindow::clear);

    time = 0;
    ui->plot->setAxes(0, 10, 0, 50);
    ui->plot->setXLabel("Time [s]", 12);
    ui->plot->setYLabel("CPM", 12);


    statusLabel = new QLabel(this);
    statusLabel->setText("Ready");
    ui->statusBar->addWidget(statusLabel);

    serialLabel = new QLabel(this);
    serialLabel->setText(serialPortDialog->getSettingsString());
    serialLabel->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(serialLabel, 2);
    //QString serialPortString = getSerialPortString();

    ui->statusBar->setMinimumHeight(20);
    ui->statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey;}" "QStatusBar::item {border: none;}");

}


void MainWindow::openSerialPortDialog()
{
    serialPortDialog->serial = serial;
    serialPortDialog->open();
}


void MainWindow::serialPortDialogFinished(int result)
{
    if(result == QDialog::Accepted)
    {
        serialLabel->setText(serialPortDialog->getSettingsString());
    }
}


void MainWindow::readData()
{
    QString data = serial->readAll();

    ui->labelCPM->setText(data);
    ui->labelDoseRate->setText(QString().sprintf("%.2f", data.toInt() * 0.057));

    X.append(10*time++);
    Y.append(data.toInt());

    if (started) {
        ui->plainTextEdit->insertPlainText(data.simplified() + " CPM\n");
        ui->plot->clear();
        ui->plot->plotXY(X, Y);
        ui->plot->setAxes(0, *std::max_element(X.constBegin(), X.constEnd()), 0, *std::max_element(Y.constBegin(), Y.constEnd())*1.2);
    }

}


void MainWindow::start()
{
    if (!started) {
        if (serial->open(QIODevice::ReadWrite)) {
            ui->buttonStart->setText("Stop");
            statusLabel->setText(tr("Started"));
            setEnabled(true);

        }
        else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
    }
    else {
        if (serial->isOpen()) {
            serial->close();
        }
        setEnabled(false);
        statusLabel->setText(tr("Ready"));
        ui->buttonStart->setText("Start");
    }
    started = !started;
}


void MainWindow::setEnabled(bool state)
{
    ui->labelCPM->setEnabled(state);
    ui->labelCPMUnit->setEnabled(state);
    ui->labelDoseRate->setEnabled(state);
    ui->labelDoseRateUnit->setEnabled(state);
}


void MainWindow::clear()
{
    ui->labelDoseRate->setText("0.00");
    ui->labelCPM->setText("0");
    ui->plainTextEdit->clear();
    X.clear();
    Y.clear();
    ui->plot->clear();
    time = 0;
}

void MainWindow::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open..."), "", tr("Text Files (*.txt);;All Files (*)"));
    QFile file(fileName);
    if (file.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd())
        {
              QString line = in.readLine();
              ui->plainTextEdit->insertPlainText(line + " CPM\n");
              X.append(i++);
              Y.append(line.toInt());
        }
        file.close();
    }
    ui->plot->plotXY(X, Y);
    ui->plot->setAxes(0, *std::max_element(X.constBegin(), X.constEnd()), 0, *std::max_element(Y.constBegin(), Y.constEnd())*1.2);
    ui->plot->update();
}


void MainWindow::fileSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), "", tr("Text Files (*.txt);;All Files (*)"));
    QFile file(fileName);
    if (file.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream out(&file);
        for (QVector<double>::const_iterator it = Y.begin(); it != Y.end(); ++it) {
            out << int(*it) << endl;
        }
        file.close();
    }
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About My Radiation Logger"),
                             tr("<b>My Radiation Logger V1.1</b><br><br>"
                                "<b>My Radiation Logger</b> displays and logs CPM and doserate data from the <b>My Electronics</b> <a href=\"http://my-electronics.net/geiger-counter-shield\">Geiger Counter Shield</a>.<br>"
                                "It is written in Qt and the source code can be found at <a href=\"http://github.com/my-electronics\">GitHub</a>."));
}


MainWindow::~MainWindow()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);

    SerialPortDialog::Settings s1 = serialPortDialog->getSettings();
    settings.setValue("PortName", s1.portNameIndex);
    settings.setValue("BaudRate", s1.baudRateIndex);
    settings.setValue("DataBits", s1.dataBitsIndex);
    settings.setValue("Parity", s1.parityIndex);
    settings.setValue("StopBits", s1.stopBitsIndex);
    settings.setValue("FlowControl", s1.flowControlIndex);

    GeigerTubeDialog::Settings s2 = geigerTubeDialog->getSettings();
    settings.setValue("ConversionFactor", s2.conversionFactor);


    serial->close();
    delete serialPortDialog;
    delete geigerTubeDialog;
    delete ui;
}
