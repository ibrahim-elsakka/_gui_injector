#include "GuiMain.h"
#include <QtWidgets/QApplication>
#include "Process.h"
#include <Windows.h>
#include <qmessagebox.h>



//fuck off
int main(int argc, char* argv[])
{


    qApp->quit();

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
