#include "noclip.hpp"
#include "../game/instance.hpp"

void c_noclip::run() {
    if (!vars::noclip::toggled) {
        // If the map is not empty, it means we were just noclipping and need to restore state.
        if (!original_collision_states.empty()) {
            for (auto const& [part, can_collide] : original_collision_states) {
                if (part) { // Check if part pointer is valid
                    instance inst(part);
                    inst.SetCanCollide(can_collide);
                }
            }
            original_collision_states.clear();
        }
        return;
    }

    // Noclip is toggled on
    if (!g_main::localplayer) {
        return;
    }

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;

    std::string local_player_name = core.get_instance_name(g_main::localplayer);
    uintptr_t character = core.find_first_child(workspace, local_player_name);
    if (!character) return;

    std::vector<uintptr_t> children = core.children(character);
    if (children.empty()) return;

    for (auto child : children) {
        std::string className = core.get_instance_classname(child);

        if (className.find("Part") != std::string::npos || className.find("Mesh") != std::string::npos) {
            instance inst(child);

            // Only store the original state if we haven't already
            if (original_collision_states.find(child) == original_collision_states.end()) {
                original_collision_states[child] = inst.GetCanCollide();
            }
            
            // Apply noclip
            inst.SetCanCollide(false);
        }
    }
}
