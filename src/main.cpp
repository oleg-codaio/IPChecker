#include <QApplication>
#include <QMessageBox>
#include "IPChecker.h"
//
int main(int argc, char ** argv)
{
    QApplication app( argc, argv );
    app.setApplicationName("IP Checker");
    app.setApplicationVersion("1.2");
    app.setOrganizationName("Oleg Vaskevich");
    while(!QSystemTrayIcon::isSystemTrayAvailable()) { //Check presence of system tray
        if(QMessageBox::critical(0, QObject::tr("app"), QObject::tr("Failed to add icon to the tray. Either your operating system doesn't have a system tray, or it has not been loaded yet. Currently, a system tray is required to run this program. Would you like to retry?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) {
            return 1;
        }
    }
    IPChecker win;
    return app.exec();
}
