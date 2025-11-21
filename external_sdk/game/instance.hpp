#pragma once
#include "../main.hpp"

class instance {
public:
    uintptr_t self;

    instance(uintptr_t address) : self(address) {}

    template <typename T>
    T read(uintptr_t address) {
        return memory->read<T>(address);
    }

    template <typename T>
    void write(uintptr_t address, T value) {
        memory->write<T>(address, value);
    }

    bool GetCanCollide();
    bool SetCanCollide(bool enable);
};
