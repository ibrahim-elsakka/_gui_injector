#include "GuiMain.h"
#include <QtWidgets/QApplication>
#include "Process.h"
#include "DownloadManager.h"

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    QStringList arguments;
    //arguments.push_back("http://speedtest.tele2.net/1MB.zip");
    arguments.push_back("https://guidedhacking.com/gh/inj/V3.3/GH Injector.zip");

    //manager.doDownload(QUrl("http://nas:80/test.zip"));
   // manager.doDownload(QUrl("http://speedtest.tele2.net/1MB.zip"));
    DownloadManager manager;
    QUrl url = ("https://guidedhacking.com/gh/inj/V3.3/GH Injector.zip");
    QString name = manager.saveFileName(url);

    manager.append(arguments);

    QObject::connect(&manager, SIGNAL(finished()), &a, SLOT(quit()));


    return a.exec();

    // Restart Application loop
    int currentExitCode = 0;
    do {
        QApplication a(argc, argv);
        GuiMain w;
        w.show();
        currentExitCode = a.exec();
    } while (currentExitCode == GuiMain::EXIT_CODE_REBOOT);

    return currentExitCode;
}
