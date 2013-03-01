#ifndef LOGGER_H
#define LOGGER_H
//
#include "ui_dlgLogger.h"
//
class Logger : public QDialog, public Ui::dlgLogger
{
Q_OBJECT
public:
    Logger(QWidget * parent = 0, Qt::WFlags f = 0);
    void appendToLog(QString data);
    QString getLast5Entries();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void askToClear();
    void updateLogCache();
};
#endif

