#include "noclip.hpp"
#include "../game/instance.hpp"

void c_noclip::run() {
    bool should_noclip = vars::noclip::toggled || vars::bss::vicious_hunter;
    
    static bool was_on = false;
    
    if (!should_noclip && !was_on) return;
    
    if (!g_main::localplayer) return;
    
    // Get character from Player::ModelInstance (0x360)
    uintptr_t character = memory->read<uintptr_t>(g_main::localplayer + offsets::Player::ModelInstance);
    if (!character) return;
    
    // Get children - ChildrenStart is at 0x70, end offset is +0x8 from the pointer
    uintptr_t children_ptr = memory->read<uintptr_t>(character + offsets::Instance::ChildrenStart);
    if (!children_ptr) return;
    
    uintptr_t start = memory->read<uintptr_t>(children_ptr);
    uintptr_t end = memory->read<uintptr_t>(children_ptr + offsets::Instance::ChildrenEnd); // 0x8
    
    if (!start || !end || end <= start) return;
    
    size_t count = (end - start) / 8;
    if (count > 30) count = 30;
    
    for (size_t i = 0; i < count; i++) {
        uintptr_t child = memory->read<uintptr_t>(start + i * 8);
        if (!child) continue;
        
        // Get primitive from the part
        uintptr_t primitive = memory->read<uintptr_t>(child + offsets::BasePart::Primitive);
        if (!primitive) continue;
        
        // Read current flags from Primitive + PrimitiveFlags offset (0x1ae is on BasePart, we need primitive offset)
        // The flags are typically at primitive + some offset, let's try reading from primitive directly
        
        // Method 1: Write to primitive flags using bitmask
        uint8_t flags = memory->read<uint8_t>(primitive + 0x1ae); // flags offset on primitive
        
        if (should_noclip) {
            flags &= ~0x8; // Clear CanCollide bit (0x8)
        } else {
            flags |= 0x8;  // Set CanCollide bit
        }
        
        memory->write<uint8_t>(primitive + 0x1ae, flags);
    }
    
    was_on = should_noclip;
}