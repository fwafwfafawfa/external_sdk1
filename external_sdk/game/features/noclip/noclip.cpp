#include "noclip.hpp"
#include "../game/instance.hpp"

void c_noclip::run() {
    if (!vars::noclip::toggled) {
        return;
    }

    if (!g_main::localplayer) {
        return;
    }

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) {
        return;
    }

    std::string local_player_name = core.get_instance_name(g_main::localplayer);
    uintptr_t character = core.find_first_child(workspace, local_player_name);
    if (!character) {
        return;
    }

    auto children = core.children(character);
    for (auto child : children) {
        instance inst(child);
        inst.SetCanCollide(false);
    }
}
