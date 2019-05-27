#include "geigertubedialog.h"
#include "ui_geigertubedialog.h"

GeigerTubeDialog::GeigerTubeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeigerTubeDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this,  &GeigerTubeDialog::accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this,  &GeigerTubeDialog::rejected);
}

GeigerTubeDialog::Settings GeigerTubeDialog::getSettings(void)
{
    return settings;
}

void GeigerTubeDialog::setSettings(Settings s)
{
    settings = s;
    updateSettings();
}

void GeigerTubeDialog::updateSettings(void)
{
    ui->spinBoxConversionFactor->setValue(settings.conversionFactor);
}

void GeigerTubeDialog::accepted()
{
    settings.conversionFactor = ui->spinBoxConversionFactor->value();
}


void GeigerTubeDialog::rejected()
{
    updateSettings();
}

GeigerTubeDialog::~GeigerTubeDialog()
{
    delete ui;
}
