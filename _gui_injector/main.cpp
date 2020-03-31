#include <QtWidgets/QApplication>
#include "GuiMain.h"
#include "DarkStyle.h"
#include "framelesswindow.h"
#include "mainwindow.h"

#include "Process.h"
#include <Windows.h>
#include <qmessagebox.h>
#include <winuser.h>

//fuck off
int main(int argc, char* argv[])
{


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
