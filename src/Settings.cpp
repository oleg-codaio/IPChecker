#include <QtCore>
#include <QtGui>
#include "Settings.h"
//
Settings::Settings(QWidget * parent, Qt::WFlags f)
        : QDialog(parent, f)
{
    setupUi(this);

    bool ok;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    txtServer->setText(settings.value("settings/server", "ftp.ipxhost.com").toString());
    spinPort->setValue(settings.value("settings/port", 21).toInt(&ok));
    if(!ok)
        spinPort->setValue(21);
    txtDir->setText(settings.value("settings/dir", "/").toString());
    txtUsername->setText(settings.value("settings/username",).toString());
    txtPassword->setText(QByteArray::fromBase64(settings.value("settings/password").toByteArray()));
    txtPrimaryIPProvider->setText(settings.value("settings/primaryIPProvider", "http://checkip.dyndns.com/index.php").toString());
    txtBackupIPProvider->setText(settings.value("settings/backupIPProvider", "").toString());

    connect(btnAutoSetup, SIGNAL(clicked()), this, SLOT(autoSetup()));
    connect(btnResetIPChecker, SIGNAL(clicked()), this, SLOT(resetIPChecker()));
    connect(btnboxDialog, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    connect(btnboxDialog, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

void Settings::saveSettings() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    settings.setValue("settings/server", txtServer->text());
    settings.setValue("settings/port", spinPort->value());
    settings.setValue("settings/dir", txtDir->text());
    settings.setValue("settings/username", txtUsername->text());
    settings.setValue("settings/password", QVariant(txtPassword->text()).toByteArray().toBase64());
    settings.setValue("settings/primaryIPProvider", txtPrimaryIPProvider->text());
    settings.setValue("settings/backupIPProvider", txtBackupIPProvider->text());
    settings.sync();
}

void Settings::autoSetup() {
    if(QMessageBox::question(this, tr("IP Checker"), tr("Are you sure you want to restore default IPXHost settings?\n\n"
                                                        "New settings won't be used until you save them."),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        txtServer->setText("");
        spinPort->setValue(21);
        txtDir->setText("/");
        txtUsername->setText("");
        txtPassword->setText("");
        txtPrimaryIPProvider->setText("http://checkip.dyndns.com/index.php");
        txtBackupIPProvider->setText("");
    }

}

void Settings::resetIPChecker() {
    if(QMessageBox::question(this, tr("IP Checker"), tr("Are you sure you want to remove all settings and variables from the hard drive, except for the log?\n\n"
                                                        "IP Checker will quit after performing the reset operation."),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
        settings.beginGroup("vars");
        settings.remove("");
        settings.endGroup();

        settings.beginGroup("settings");
        settings.remove("");
        settings.endGroup();

        settings.sync();
        qApp->quit();
    }
}

void Settings::showHelp() {
    QMessageBox::information(this, "IP Checker",
                             "<center><u>IP Checker Help</u></center><br>"
                             "The settings page allows you to customize IP Checker:<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Server:</b> The FTP server/host address. (e.g. ftp.website.com)<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Port:</b> The FTP port. (e.g. 21)<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Dir:</b> The directory that IP Checker should change to on the FTP Server.<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Username:</b> The user that IP Checker should use to log in. (e.g. user@website.com)<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Password:</b> The password that IP checker should use.<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Primary IP Provider:</b> The web page location (URL) used for retrieving the IP Address. "
                             "The default is <a href=\"http://checkip.dyndns.com/index.php\">http://checkip.dyndns.com/index.php</a>. Please note that only "
                             "the first IP address from the IP Provider is extracted. It is based on \"###.###.###.###\", where each ### is 1-3 numbers.<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Backup IP Provider:</b> The IP provider to be used if the primary provider cannot be reached.  "
                             "If blank, a backup IP Provider is not used.<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>IPXHost Auto-Setup:</b> Clicking this button will prompt you to replace all values with the ones used in "
                             "IP Checker v1.0.<br>"
                             "&nbsp;&nbsp;&nbsp;&nbsp;<b>Reset IP Checker:</b> Clicking this button will prompt you to remove all settings and application variables used, except the log, from the hard drive.");
}
