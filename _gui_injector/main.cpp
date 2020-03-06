#include "GuiMain.h"
#include <QtWidgets/QApplication>
#include "Process.h"

int main(int argc, char* argv[])
{
    Process_Struct p1 = getProcessByName("HxD64.exe");
    Process_Struct p2 = getProcessByPID(2528);
    int i = 42;


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
