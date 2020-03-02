#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include <fstream>
#include "process.h"


int getFileArch(const char* szDllFile)
{
    BYTE* pSrcData = nullptr;
    IMAGE_NT_HEADERS* pOldNtHeader = nullptr;
    IMAGE_FILE_HEADER* pOldFileHeader = nullptr;

    if (!GetFileAttributesA(szDllFile))
    {
        printf("File doesn't exist\n");
        return false;
    }

    std::ifstream File(szDllFile, std::ios::binary | std::ios::ate);

    if (File.fail())
    {
        printf("Opening the file failed: %X\n", (DWORD)File.rdstate());
        File.close();
        return false;
    }

    auto FileSize = File.tellg();
    if (FileSize < 0x1000)
    {
        printf("Filesize is invalid.\n");
        File.close();
        return false;
    }

    pSrcData = new BYTE[static_cast<UINT_PTR>(FileSize)];
    if (!pSrcData)
    {
        printf("Memory allocating failed\n");
        File.close();
        return false;
    }

    File.seekg(0, std::ios::beg);
    File.read(reinterpret_cast<char*>(pSrcData), FileSize);
    File.close();

    if (reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D) //"MZ"
    {
        printf("Invalid file\n");
        delete[] pSrcData;
        return false;
    }

    pOldNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(pSrcData + reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_lfanew);
    pOldFileHeader = &pOldNtHeader->FileHeader;

    if (pOldFileHeader->Machine == IMAGE_FILE_MACHINE_AMD64)
    {
        delete[] pSrcData;
        return X64;
    }
    else if (pOldFileHeader->Machine == IMAGE_FILE_MACHINE_I386)
    {
        delete[] pSrcData;
        return X86;
    }

    delete[] pSrcData;
    return false;
}


bool getProcessList(std::vector<process_list>& pl)
{

    PROCESSENTRY32 procEntry = { 0 };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (hSnapshot)
    {
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &procEntry))
        {
            do
            {
                process_list pl_item;
                pl_item.pid = procEntry.th32ProcessID;
                int ret = wcstombs(pl_item.name, procEntry.szExeFile, sizeof(pl_item.name));
                pl_item.arch = NONE;

                HANDLE hOpenProc = OpenProcess(PROCESS_QUERY_INFORMATION, NULL, procEntry.th32ProcessID);
                if (hOpenProc != NULL)
                {
                    BOOL tempWow64 = FALSE;

                    BOOL bIsWow = IsWow64Process(hOpenProc, &tempWow64);
                    if (bIsWow != 0)
                    {
                        if (tempWow64 == TRUE)
                        {
                            pl_item.arch = X64;
                        }
                        else
                        {
                            pl_item.arch = X86;
                        }
                    }
                    CloseHandle(hOpenProc);

                    pl.push_back(pl_item);
                }

            } while (Process32Next(hSnapshot, &procEntry));
        }
    }
    CloseHandle(hSnapshot);
    return true;
}

process_list getProcessByName(const char* proc)
{

    PROCESSENTRY32 procEntry = { 0 };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    char name[250]; 
    process_list pl_item;
    pl_item.arch = NONE;
    pl_item.pid = 0;

    if (hSnapshot)
    {
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &procEntry))
        {
            do
            {
                int ret = wcstombs(name, procEntry.szExeFile, sizeof(name));
                if (!strcmp(name, proc))
                {

                    HANDLE hOpenProc = OpenProcess(PROCESS_QUERY_INFORMATION, NULL, procEntry.th32ProcessID);
                    if (hOpenProc != NULL)
                    {
                        BOOL tempWow64 = FALSE;

                        BOOL bIsWow = IsWow64Process(hOpenProc, &tempWow64);
                        if (bIsWow != 0)
                        {
                            if (tempWow64 == TRUE)
                            {
                                pl_item.arch = X64;
                            }
                            else
                            {
                                pl_item.arch = X86;
                            }
                            pl_item.pid = procEntry.th32ProcessID;
                            strcpy(pl_item.name, name);
                            break;
                        }
                        CloseHandle(hOpenProc);

                    }
                }

            } while (Process32Next(hSnapshot, &procEntry));
        }
    }
    CloseHandle(hSnapshot);
    return pl_item;
}

int getProcArch(int pid)
{
    HANDLE hOpenProc = OpenProcess(PROCESS_QUERY_INFORMATION, NULL, pid);
    if (hOpenProc != NULL)
    {
        BOOL tempWow64 = FALSE;

        BOOL bIsWow = IsWow64Process(hOpenProc, &tempWow64);
        if (bIsWow != 0)
        {
            if (tempWow64 == TRUE)
            {
                return X86;
            }
            else
            {
                return X64;
            }
            return NONE;
        }
        CloseHandle(hOpenProc);

    }
    return NONE;
}
