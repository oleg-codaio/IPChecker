#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "ui_dlgSettings.h"

class Settings : public QDialog, public Ui::dlgSettings
{
Q_OBJECT

public:
    Settings(QWidget * parent = 0, Qt::WFlags f = 0);

public slots:
    void saveSettings();

private slots:
    void autoSetup();
    void resetIPChecker();
    void showHelp();
};

#endif // SETTINGS_H
