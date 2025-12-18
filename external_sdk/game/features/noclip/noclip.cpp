#include "noclip.hpp"
#include "../game/instance.hpp"

void c_noclip::run() {
    static uintptr_t last_character = 0;
    static std::chrono::steady_clock::time_point last_scan = std::chrono::steady_clock::now();

    if (!vars::noclip::toggled) {
        if (!original_collision_states.empty()) {
            for (auto const& [part, can_collide] : original_collision_states) {
                if (part) {
                    instance inst(part);
                    inst.SetCanCollide(can_collide);
                }
            }
            original_collision_states.clear();
            last_character = 0;
        }
        return;
    }

    if (!g_main::localplayer) {
        return;
    }

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;

    std::string local_player_name = core.get_instance_name(g_main::localplayer);
    uintptr_t character = core.find_first_child(workspace, local_player_name);
    if (!character) return;

    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - last_scan).count();

    // Only rescan if character changed or 2 seconds passed
    if (character != last_character || elapsed > 2.0f) {
        original_collision_states.clear();

        std::vector<uintptr_t> children = core.children(character);

        for (auto child : children) {
            std::string className = core.get_instance_classname(child);

            if (className.find("Part") != std::string::npos || className.find("Mesh") != std::string::npos) {
                instance inst(child);

                if (original_collision_states.find(child) == original_collision_states.end()) {
                    original_collision_states[child] = inst.GetCanCollide();
                }
            }
        }

        last_character = character;
        last_scan = now;
    }

    // Apply noclip to cached parts
    for (auto const& [part, _] : original_collision_states) {
        if (part) {
            instance inst(part);
            inst.SetCanCollide(false);
        }
    }
}
