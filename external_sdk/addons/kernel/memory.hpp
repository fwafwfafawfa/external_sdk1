#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <string>
#include <vector>
#include <Psapi.h>

class c_memory {
public:
    c_memory(const char* process_name);
    ~c_memory();

    uintptr_t find_image();
    int32_t find_process(const char* process_name);

    template < typename T >
    T read(uintptr_t address) {
        T buffer{};
        ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), nullptr);
        return buffer;
    }

    template < typename T >
    void write(uintptr_t address, const T& buffer) {
        WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(address), &buffer, sizeof(T), nullptr);
    }

private:
    HANDLE process_handle;
    int32_t process_id;
};

inline c_memory* memory;