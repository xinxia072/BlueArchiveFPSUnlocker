#include <windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include <filesystem>

bool inject(DWORD pid, const wchar_t* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        printf("OpenProcess: %ld\n", GetLastError());
        return false;
    }

    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, wcslen(dllPath) * sizeof(wchar_t) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pDllPath == NULL) {
        printf("VirtualAllocEx: %ld\n", GetLastError());
        return false;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, dllPath, wcslen(dllPath) * sizeof(wchar_t) + 1, NULL)) {
        printf("WriteProcessMemory: %ld\n", GetLastError());
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, pDllPath, 0, NULL);
    if (hThread == NULL) {
        printf("CreateRemoteThread: %ld\n", GetLastError());
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

DWORD pidFromName(const char* processName) {
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (strcmp(processName, pe.szExeFile) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    DWORD pid = 0;
    printf("Waiting for ba");
    while (!pid) {
        pid = pidFromName("BlueArchive.exe");
        Sleep(50);
    }

    const wchar_t* dll = (std::filesystem::current_path() / "unlocker.dll").c_str();
    if (inject(pid, dll)) {
        printf("1kfps!!!");
    }

    return 0;
}
