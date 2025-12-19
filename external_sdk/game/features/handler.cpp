#include "handler.hpp"
#include "example_esp/esp.hpp"
#include "speed_hack/speed.hpp"
#include "freecam/freecam.hpp"
#include "fly/fly.hpp"
#include "infinite_jump/infinite_jump.hpp"
#include "airswim/airswim.hpp"
#include "../../tphandler.hpp"  // Add this

#include "../../handlers/vars.hpp"
#include "../../handlers/misc/misc.hpp"
#include <chrono>

void c_feature_handler::start(uintptr_t datamodel)
{
    // MASTER SAFETY - Check this FIRST before ANY memory access
    if (vars::bss::is_hopping) return;
    if (!memory) return;
    if (!g_main::datamodel) return;
    if (!g_main::localplayer) return;
    if (!tp_handler.is_ready()) return;

    // Delta time
    static auto last_time = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> dt = now - last_time;
    last_time = now;

    if (!datamodel && !g_main::v_engine) return;

    auto roblox_window = FindWindowA(NULL, "Roblox");
    if (!roblox_window || GetForegroundWindow() != roblox_window) return;

    // Double check before reading viewmatrix
    if (!g_main::v_engine) return;

    view_matrix_t viewmatrix = memory->read<view_matrix_t>(g_main::v_engine + offsets::viewmatrix);

    if (vars::esp::toggled)
        esp.run_players(viewmatrix);

    esp.run_aimbot(viewmatrix);
    esp.draw_hitbox_esp(viewmatrix);
    esp.run_vicious_esp(viewmatrix);
    esp.run_vicious_hunter();
    esp.float_to_target();
    esp.test_hive_claiming();

    speed_hack::run();

    freecam.enabled = vars::freecam::toggled;
    freecam.run(dt.count());  // Now dt is defined

    jump_power::run();
    infinite_jump::run();
    airswim.run();

    if (vars::fly::toggled)
        fly.run();

    if (vars::set_fov::unlock_zoom)
        freecam.unlock_zoom();

    if (vars::set_fov::toggled)
        freecam.set_fov(vars::set_fov::set_fov);

    misc.run_anti_afk();
}