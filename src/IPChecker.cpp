#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include "IPChecker.h"
#include "Settings.h"
#include "Logger.h"
//
IPChecker::IPChecker(QWidget * parent, Qt::WFlags f) 
    : QWidget(parent, f)
{
    setupUi(this);
    move(50, 50);

    nAM = new QNetworkAccessManager(this);
    fnAM = new QNetworkAccessManager(this);

    logger = new Logger(this, Qt::Dialog);
    ipsettings = new Settings(this, Qt::Dialog);

//    timeoutTimer = new QTimer(this);
//    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(cancelProcedure()));

    tryNewHost = false;
    IPHasChanged = false;
    marker = false;
    errorHappened = false;
    ftpErrorHappened = false;
    ftpErrorHappenedNewIP = false;
    timingInProgress = false;
    killTimer = false;
    dontAllowProcedure2 = false;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    txtCurrentIP->setText(settings.value("vars/lastIP").toString());
    txtCompName->setText(settings.value("vars/compName").toString());
    spinInterval->setValue(settings.value("vars/interval", "5").toInt());

    if(settings.value("vars/crashBool").toBool()) {
        logger->appendToLog(tr("%1 - <i>Either the program was terminated or the computer was reset"
                               " sometime between the last two log entry times.</i>")
                            .arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP")));
    }
    settings.setValue("vars/crashBool", true);
    settings.sync();

    if(settings.value("settings/server").toString().isEmpty()) {
        ipsettings->saveSettings();
    }

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(eraseCrashBool()));
    connect(lblHelpMsg, SIGNAL(linkActivated(QString)), this, SLOT(showColorCodes(QString)));
    connect(btnStart, SIGNAL(clicked()), this, SLOT(doProcedure1()));
    connect(btnSettings, SIGNAL(clicked()), this, SLOT(showSettings()));
    connect(btnViewLog, SIGNAL(clicked()), this, SLOT(showLogger()));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(hide()));
    connect(txtCurrentIP, SIGNAL(textChanged(QString)), this, SLOT(updateSettings()));
    connect(txtCompName, SIGNAL(textChanged(QString)), this, SLOT(updateSettings()));
    connect(spinInterval, SIGNAL(valueChanged(QString)), this, SLOT(updateSettings()));

    showDialogAction = new QAction(tr("Show/hide window"), this);
    connect(showDialogAction, SIGNAL(triggered()), this, SLOT(setDialogVisible()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showDialogAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icon.png"));

    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    if(txtCompName->text().size() >= 3) {
        doProcedure1();
    }
    else {
        show();
    }
}

void IPChecker::closeEvent(QCloseEvent *event) { //Prevents the user from entirely exiting the program when he just closes the dialog
    hide();
    event->ignore();
}

void IPChecker::updateBlink() {
    if(killTimer) {
        killTimer = false;
        return;
    }
    if(txtCurrentIP->styleSheet() == "QLineEdit{background:yellow; color:black;}") {
        txtCurrentIP->setStyleSheet("QLineEdit{background:red; color:white;}");
    }
    else {
        txtCurrentIP->setStyleSheet("QLineEdit{background:yellow; color:black;}");
    }
    QTimer::singleShot(500, this, SLOT(updateBlink()));
}

void IPChecker::setDialogVisible() { //Toggles dialog visibility
    if(isVisible()) {
        hide();
    } else {
        show(); //Quick way of showing/hiding dialog (A)?B:C
        raise();
    }
}

void IPChecker::iconActivated(QSystemTrayIcon::ActivationReason reason) { //Event handler for tray icon
    switch (reason) {
        case QSystemTrayIcon::Trigger: {
            setDialogVisible();
            break;
        }
        case QSystemTrayIcon::DoubleClick:{
            break;
        }
        case QSystemTrayIcon::MiddleClick: {
            break;
        }
        default: {
            ;
        }
    }
}

void IPChecker::updateSettings() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    settings.setValue("vars/lastIP", txtCurrentIP->text());
    settings.setValue("vars/compName", txtCompName->text());
    settings.setValue("vars/interval", spinInterval->value());
    settings.sync();
}

void IPChecker::showLogger() {
    logger->show();
}

void IPChecker::showSettings() {
    ipsettings->show();
}

void IPChecker::doProcedure1() {
    connect(nAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(doProcedure2(QNetworkReply*)));
//    timeoutTimer->start(10000);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    QUrl primaryIPProvider(settings.value("settings/primaryIPProvider", "http://checkip.dyndns.com/index.php").toString());
    QUrl backupIPProvider(settings.value("settings/backupIPProvider").toString());

    if(!btnStart->isEnabled() && !marker) {
        return;
    }
    marker = false;
    if(txtCompName->text().size() < 3) {
        QMessageBox::warning(this, tr("IP Checker"), tr("Please fill in a proper computer name."));
        return;
    }
    if(!tryNewHost) {
        beginBlinking();
    }
    dontAllowProcedure2 = false;
    QNetworkRequest request;
    if(tryNewHost) {
        request.setUrl(backupIPProvider);
        httpReply = nAM->get(request);
    }
    else {
        request.setUrl(primaryIPProvider);
        httpReply = nAM->get(request);
    }
}

void IPChecker::doProcedure1AndResetTimer() {
    timingInProgress = false;
    doProcedure1();
}

void IPChecker::doProcedure2(QNetworkReply *pReply) {
    if (dontAllowProcedure2)
        return;
    disconnect(nAM);
    dontAllowProcedure2 = true;

    if(pReply->error() == QNetworkReply::OperationCanceledError)
        return;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    QUrl primaryIPProvider(settings.value("settings/primaryIPProvider", "http://checkip.dyndns.com/index.php").toString());
    QUrl backupIPProvider(settings.value("settings/backupIPProvider", "").toString());
    QString server(settings.value("settings/server", "").toString());
    int port(settings.value("settings/port", 21).toInt());
    QString dir(settings.value("settings/dir", "/").toString());
    QString username(settings.value("settings/username", "").toString());
    QString password(QByteArray::fromBase64(settings.value("settings/password", "").toByteArray()));

    if(pReply->error() != QNetworkReply::NoError) {
        if(tryNewHost) {
            logger->appendToLog(tr("%1 - <i>Could not contact %2 either: %3.</i>")
                                .arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP"))
                                .arg(backupIPProvider.host()).arg(pReply->errorString()));
            errorHappened = true;
            addPendingProcedure(true);
            return;
        }
        else {
            logger->appendToLog(tr("%1 - <i>Could not contact %2: %3. Attempting to contact %4.</i>")
                                .arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP"))
                                .arg(primaryIPProvider.host()).arg(pReply->errorString())
                                .arg(backupIPProvider.host()));
            tryNewHost = true;
            marker = true;
            doProcedure1();
            return;
        }
    }

    pReply->open(QIODevice::ReadWrite);
    QString IP = pReply->readAll();

    //Make sure this file contains an IP address
    QRegExp ipRegExp("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
    qDebug() << "IP: " << IP;
    if(ipRegExp.indexIn(IP) == -1) {
        if(!tryNewHost) {
            logger->appendToLog(tr("%1 - <i>Invalid data retrieved from %2. Attempting to contact %3.</i>")
                                .arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP"))
                                .arg(primaryIPProvider.host()).arg(backupIPProvider.host()));
            tryNewHost = true;
            marker = true;
            doProcedure1();
            return;
        }
        else {
            logger->appendToLog(tr("%1 - <i>Invalid data retrieved from %2 too.</i>")
                                .arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP"))
                                .arg(backupIPProvider.host()));
            errorHappened = true;
            addPendingProcedure(true);
            return;
        }
    }
    IP = ipRegExp.cap();
    if(tryNewHost)
        tryNewHost = false;
    if(IP != txtCurrentIP->text())
        IPHasChanged = true;
    txtCurrentIP->setText(IP);
    QString last5Entries = logger->getLast5Entries();
    QByteArray msg;
    msg.append("Hello, this is an automated file uploaded by IP Checker "
               + QCoreApplication::applicationVersion() + ".\n\nComputer Name: "
               + txtCompName->text() + "\nCurrent IP Address: " + IP + "\nDate and Time Sent: "
               + QDateTime::currentDateTime().toString("dddd MMMM d, yyyy - h:mm:ss AP")
               + "\n\nIf the current time and date is within " + spinInterval->value()
               + " minutes of the latest log entry, everything should be OK. Here "
               "are the 5 most recent log entries:\n\n" + last5Entries
               + "\n\n\nThank you for choosing IP Checker. :-)");

    connect(fnAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(ftpCommandFinished(QNetworkReply*)));
    QNetworkRequest fRequest;
    QUrl url;
    url.setScheme("ftp");
    url.setUserName(username);
    url.setPassword(password);
    url.setHost(server);
    url.setPort(port);
    url.setPath(dir + txtCompName->text() + ".txt");

    fRequest.setUrl(url);
    ftpReply = fnAM->put(fRequest, msg);

    pReply->deleteLater();
}

void IPChecker::ftpCommandFinished(QNetworkReply *fReply) {
    disconnect(fnAM);
    if(fReply->error() != QNetworkReply::NoError) {
        if(IPHasChanged) {
            ftpErrorHappenedNewIP = true;
            logger->appendToLog(tr("%1 - <i>The IP Address has updated to <b>%2</b>, but there was an error connecting "
                                   "to the FTP server: %3.</i>").arg(QDateTime::currentDateTime().toString(
                                           "M/d/yy h:mm:ssAP")).arg(txtCurrentIP->text()).arg(fReply->errorString()));
            fReply->deleteLater();
        }
        else {
            ftpErrorHappened = true;
            logger->appendToLog(tr("%1 - <i>There was an error connecting "
                       "to the FTP server: %2.</i>").arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP")).arg(
                               fReply->errorString()));
        }
        addPendingProcedure(true);
        fReply->deleteLater();
        return;
    }
    else {
        if(IPHasChanged) {
            logger->appendToLog(tr("%1 - <i>The IP Address has updated to <b>%2</b> and the file has been "
                                   "uploaded to the FTP server.</i>").arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP")).arg(txtCurrentIP->text()));
        }
        addPendingProcedure(true);
        fReply->deleteLater();
        return;
    }
}

void IPChecker::addPendingProcedure(bool) {
//    timeoutTimer->stop();
    if(!timingInProgress) {
        int interval = spinInterval->value() * 60000;
        QTimer::singleShot(abs(interval), this, SLOT(doProcedure1AndResetTimer()));
    }
    timingInProgress = true;
    tryNewHost = false;
    stopBlinking();
}

void IPChecker::eraseCrashBool() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Oleg Vaskevich", "IP Checker", this);
    settings.setValue("vars/crashBool", false);
    settings.sync();
}

void IPChecker::cancelProcedure() {
    qDebug() << "canceling";
    logger->appendToLog(tr("%1 - <i>The process timed out (10 seconds).</i>").arg(QDateTime::currentDateTime().toString("M/d/yy h:mm:ssAP")));

//    nAM->disconnect();
//    httpReply->abort();
//    ftpReply->abort();

    errorHappened = true;
    addPendingProcedure(true);
}

void IPChecker::showColorCodes(QString) {
    QMessageBox colorCodes(this);
    colorCodes.setWindowTitle("IP Checker Color Codes");
    colorCodes.setText("The IP Box background changes colors depending on the program status:<br><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:red; background:yellow;\">Blinking red/yellow: Processing</span><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:white; background:green;\">Green: IP Changed</span><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:black; background:yellow;\">Yellow: No change in IP</span><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:white; background:red;\">Red: Error retrieving IP</span><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:white; background:purple;\">Purple: FTP Error; IP Changed</span><br>"
                       "&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"color:white; background:orange;\">Orange: FTP Error; no change in IP</span><br><br>"
                       "Please note that the program status is based only on the most recent check.");
    colorCodes.setTextFormat(Qt::RichText);
    colorCodes.setIcon(QMessageBox::Information);
    colorCodes.setWindowIcon(QIcon(":/icon.png"));
    colorCodes.exec();
}

void IPChecker::beginBlinking() {
    txtCurrentIP->setStyleSheet("QLineEdit{background:red; color:white;}");
    QTimer::singleShot(500, this, SLOT(updateBlink()));
    txtCompName->setEnabled(false);
    spinInterval->setEnabled(false);
    btnStart->setEnabled(false);
    btnSettings->setEnabled(false);
}

void IPChecker::stopBlinking() {
    killTimer = true;
    if(errorHappened) {
        txtCurrentIP->setStyleSheet("QLineEdit{background:red; color:white;}");
        errorHappened = false;
    }
    else if(ftpErrorHappened) {
        txtCurrentIP->setStyleSheet("QLineEdit{background:orange; color:white;}");
        ftpErrorHappened = false;
    }
    else if(ftpErrorHappenedNewIP) {
        txtCurrentIP->setStyleSheet("QLineEdit{background:purple; color:white;}");
        ftpErrorHappenedNewIP = false;
    }
    else if(IPHasChanged) {
        txtCurrentIP->setStyleSheet("QLineEdit{background:green; color:white;}");
    }
    else {
        txtCurrentIP->setStyleSheet("QLineEdit{background:yellow; color:black;}");
    }
    IPHasChanged = false;
    txtCompName->setEnabled(true);
    spinInterval->setEnabled(true);
    btnStart->setEnabled(true);
    btnSettings->setEnabled(true);
}

//void IPChecker::showNewState(int state) {
//    qDebug() << "new State: " << state;
//}
