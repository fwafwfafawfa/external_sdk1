#include "handler.hpp"
#include "example_esp/esp.hpp"
#include "speed_hack/speed.hpp"
#include "freecam/freecam.hpp"
#include "fly/fly.hpp"
#include "infinite_jump/infinite_jump.hpp"
#include "airswim/airswim.hpp"

#include "../../handlers/vars.hpp"
#include "../../handlers/misc/misc.hpp"
#include <chrono>

void c_feature_handler::start(uintptr_t datamodel)
{
    static auto last_time = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> dt = now - last_time;
    last_time = now;

    if (!g_main::datamodel || !g_main::localplayer)
        return;

    if (!datamodel && !g_main::v_engine)
        return;

    auto roblox_window = FindWindowA(NULL, "Roblox");
    if (!roblox_window || GetForegroundWindow() != roblox_window)
        return;

    view_matrix_t viewmatrix = memory->read<view_matrix_t>(g_main::v_engine + offsets::viewmatrix);

    if (vars::esp::toggled)
        esp.run_players(viewmatrix);

    esp.run_aimbot(viewmatrix);
    esp.draw_hitbox_esp(viewmatrix);
    esp.run_vicious_esp(viewmatrix);
    esp.run_vicious_hunter();
    esp.float_to_target();


    speed_hack::run();

    freecam.enabled = vars::freecam::toggled;
    freecam.run(dt.count());

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
