#pragma once

enum ARCH
{
    NONE,
    X64,
    X86
};

struct process_list
{
    unsigned long   pid;
    char            name[100];
    int             arch;
};

int getFileArch(const char* szDllFile);
bool getProcessList(std::vector<process_list>& pl);
process_list getProcessByName(const char* name);
int getProcArch(int pid);