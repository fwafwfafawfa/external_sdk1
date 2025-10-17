#include "instance.hpp"

bool instance::GetCanCollide() {
    uintptr_t Primitive = driver.read<uintptr_t>(this->self + offsets::Primitive);
    if (!Primitive) return false;
    return (driver.read<BYTE>(Primitive + offsets::CanCollide) & 0x08) != 0;
}

bool instance::SetCanCollide(bool enable) {
    uintptr_t Primitive = driver.read<uintptr_t>(this->self + offsets::Primitive);
    if (!Primitive) return false;

    BYTE val = driver.read<BYTE>(Primitive + offsets::CanCollide);

    if (enable)
        val |= 0x08;
    else
        val &= ~0x08;

    driver.write<BYTE>(Primitive + offsets::CanCollide, val);
    return enable;
}
