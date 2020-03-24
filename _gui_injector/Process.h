#pragma once

enum ARCH
{
    NONE,
    X64,
    X86
};

struct Process_Struct
{
    unsigned long   pid;
    char            name[100];
    int             arch;
};

enum ARCH getFileArch(const char* szDllFile);
enum ARCH getProcArch(const int pid);
Process_Struct getProcessByName(const char* name);
Process_Struct getProcessByPID(const int pid);
bool getProcessList(std::vector<Process_Struct>& pl);

bool SetDebugPrivilege(bool Enable);