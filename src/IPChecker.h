#ifndef IPCHECKER_H
#define IPCHECKER_H
//
#include <QWidget>
#include <QSystemTrayIcon>
#include "ui_dlgIPChecker.h"
#include "Settings.h"
//
class QAction;
class QMenu;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QBuffer;
class QTextStream;
class Logger;
class IPChecker : public QWidget, public Ui::dlgIPChecker
{
Q_OBJECT
public:
    IPChecker(QWidget * parent = 0, Qt::WFlags f = 0);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void updateBlink();
    void setDialogVisible();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void updateSettings();
    void showSettings();
    void showLogger();
    void doProcedure1();
    void doProcedure1AndResetTimer();
    void doProcedure2(QNetworkReply *hReply);
    void ftpCommandFinished(QNetworkReply *fReply);
    void addPendingProcedure(bool);
    void eraseCrashBool();
    void cancelProcedure();
    void showColorCodes(QString s);
//    void showNewState(int state);
private:
    QAction *showDialogAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void beginBlinking();
    void stopBlinking();

    bool killTimer;

    Logger *logger;
    Settings *ipsettings;

    QNetworkAccessManager *nAM;
    QNetworkAccessManager *fnAM;
    QBuffer *buffer;
    QTextStream *textStream;
    QNetworkReply *httpReply;
    QNetworkReply *ftpReply;
    bool tryNewHost;

    QTimer *timeoutTimer;

    bool IPHasChanged;
    bool errorHappened;
    bool ftpErrorHappened;
    bool ftpErrorHappenedNewIP;

    bool marker;
    bool timingInProgress;
    bool dontAllowProcedure2;
};
#endif



