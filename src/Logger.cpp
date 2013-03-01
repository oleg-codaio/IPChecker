#include <QtCore>
#include <QtGui>
#include "Logger.h"

Logger::Logger(QWidget * parent, Qt::WFlags f) 
    : QDialog(parent, f)
{
    setupUi(this);
    move(280, 50);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    if(settings.value("Log").toString().isEmpty()) {
            settings.setValue("Log", "This is a log of the activity of this program:\n");
    }
    txtLogger->setHtml(settings.value("Log").toString());
    connect(txtLogger, SIGNAL(textChanged()), this, SLOT(updateLogCache()));
    connect(btnClear, SIGNAL(clicked()), this, SLOT(askToClear()));
    txtLogger->moveCursor(QTextCursor::End);
    txtLogger->ensureCursorVisible();
}

void Logger::appendToLog(QString data) {
    txtLogger->append(data);
}

QString Logger::getLast5Entries() {
    return txtLogger->toPlainText().section("\n", txtLogger->toPlainText().count("\n") - 5);
}

void Logger::closeEvent(QCloseEvent *event) { //Prevents the user from entirely exiting the program when he just closes the dialog
    hide();
    event->ignore();
}

void Logger::askToClear() {
    if(QMessageBox::question(this, tr("IP Checker Logger"), tr("Are you sure you want to clear the log?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        txtLogger->clear();
        txtLogger->append("This is a log of the activity of this program:\n");
    }
}

void Logger::updateLogCache() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    settings.setValue("Log", txtLogger->toHtml());
    settings.sync();
    txtLogger->moveCursor(QTextCursor::End);
    txtLogger->ensureCursorVisible();
}


