#pragma once
#include "../main.hpp"

class instance {
public:
    uintptr_t self;

    instance(uintptr_t address) : self(address) {}

    bool GetCanCollide();
    bool SetCanCollide(bool enable);
};
