#include "memory.hpp"

c_memory::c_memory(const char* process_name) {
    process_id = find_process(process_name);
    if (process_id) {
        process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    }
}

c_memory::~c_memory() {
    if (process_handle) {
        CloseHandle(process_handle);
    }
}

int32_t c_memory::find_process(const char* process_name) {
    PROCESSENTRY32 process_entry{};
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(snapshot, &process_entry)) {
        do {
            if (_stricmp(process_entry.szExeFile, process_name) == 0) {
                CloseHandle(snapshot);
                return process_entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &process_entry));
    }

    CloseHandle(snapshot);
    return 0;
}

uintptr_t c_memory::find_image() {
    HMODULE modules[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(process_handle, modules, sizeof(modules), &cbNeeded)) {
        return (uintptr_t)modules[0];
    }
    return 0;
}