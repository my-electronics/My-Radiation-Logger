#ifndef GEIGERTUBEDIALOG_H
#define GEIGERTUBEDIALOG_H

#include <QDialog>

namespace Ui {
class GeigerTubeDialog;
}

class GeigerTubeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeigerTubeDialog(QWidget *parent = nullptr);
    ~GeigerTubeDialog();

    struct Settings {
        double conversionFactor;
    };

    Settings settings;

    Settings getSettings(void);
    void setSettings(Settings);
    void updateSettings(void);

private slots:
    void accepted();
    void rejected();

private:
    Ui::GeigerTubeDialog *ui;
};

#endif // GEIGERTUBEDIALOG_H
