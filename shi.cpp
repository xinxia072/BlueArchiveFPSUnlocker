#include <windows.h>
#include <cstdint>
#include <iostream>

typedef void(__stdcall *set_fps_t)(int fps);
void shit(HMODULE hModule) {
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

    uint64_t set_fps_offset = game_assembly + 0x763D750; // UnityEngine.Application.set_targetFrameRate
    set_fps_t set_fps = (set_fps_t)(set_fps_offset);
    set_fps(240);

    // prevent game setting fps limit
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(set_fps_offset), 1, PAGE_EXECUTE_READWRITE, &oldProtect);
    *reinterpret_cast<BYTE*>(set_fps_offset) = 0xC3; //old is 0x56
    VirtualProtect(reinterpret_cast<LPVOID>(set_fps_offset), 1, oldProtect, &oldProtect);

    // unload dll
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // meow cutely (permanent banned)
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)shit, hModule, 0, NULL);
    }
    return 1;
}
