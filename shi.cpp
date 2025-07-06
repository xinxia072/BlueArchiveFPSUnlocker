#include <windows.h>
#include <cstdint>
#include <iostream>

typedef void(__stdcall *set_fps_t)(int fps);
void shit() {
    //AllocConsole();
    //FILE* pConsole;
    //freopen_s(&pConsole, "CONOUT$", "w", stdout);
    //freopen_s(&pConsole, "CONIN$", "r", stdin);

    uint64_t game_assembly = 0;
    while (!game_assembly) {
        game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("GameAssembly.dll"));
        Sleep(100);
    }
    //printf("found gameassembly.dl;\n");
    set_fps_t set_fps = (set_fps_t)(game_assembly + 0x763CD40);

    while (1) {
        set_fps(-1);
        Sleep(5000);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // meow cutely (permanent banned)
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)shit, NULL, 0, NULL);
    }
    return 1;
}
