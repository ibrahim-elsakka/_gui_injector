#include "GuiMain.h"
#include <QtWidgets/QApplication>

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
