#include "instance.hpp"

// This implementation now uses the PrimitiveFlags offset and CanCollideMask,
// which is more likely to be correct after a game update.
bool instance::GetCanCollide() {
    uintptr_t Primitive = read<uintptr_t>(this->self + offsets::Primitive);
    if (!Primitive) return false;

    // Read the entire flags byte
    BYTE flags = read<BYTE>(Primitive + offsets::BasePart::PrimitiveFlags);

    // Check if the CanCollide bit is set
    return (flags & offsets::CanCollideMask) != 0;
}

bool instance::SetCanCollide(bool enable) {
    uintptr_t Primitive = read<uintptr_t>(this->self + offsets::Primitive);
    if (!Primitive) return false;

    // Read the entire flags byte
    BYTE flags = read<BYTE>(Primitive + offsets::BasePart::PrimitiveFlags);

    if (enable)
        flags |= offsets::CanCollideMask; // Set the CanCollide bit
    else
        flags &= ~offsets::CanCollideMask; // Unset the CanCollide bit

    // Write the modified flags byte back
    write<BYTE>(Primitive + offsets::BasePart::PrimitiveFlags, flags);
    return enable;
}
