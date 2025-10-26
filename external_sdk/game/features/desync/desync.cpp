#include "../../../handlers/vars.hpp"
#include <thread>
#include <chrono>
#include "desync.hpp"
#include "../../offsets/offsets.hpp"
#include "../../../addons/kernel/memory.hpp"
#include "misc/misc.hpp"

using namespace vars; // Use vars namespace for misc

namespace hooks {
void desync() {
    static bool was_enabled = false;
    static int32_t original_value = -1;
    static auto restore_start_time = std::chrono::steady_clock::now();
    static bool is_restoring = false;
    static bool has_been_used = false;

    while (true) {
        // Replace globals::firstreceived with true for now, or add your own check if needed
        if (false) {
            if (has_been_used && original_value != -1) {
                // Replace write with memory->write if needed
                // write<int32_t>(base_address + offsets::PhysicsSenderMaxBandwidthBps, original_value);
                original_value = -1;
            }
            return;
        }

        misc::desynckeybind.update();
        // Allow desync to be enabled either by the UI toggle or the keybind
        bool currently_enabled = misc::desync || misc::desynckeybind.enabled;

        if (currently_enabled && !was_enabled) {
            if (original_value == -1) {
                // Replace read with memory->read if needed
                // original_value = read<int32_t>(base_address + offsets::PhysicsSenderMaxBandwidthBps);
                has_been_used = true;
            }
            // write<int32_t>(base_address + offsets::PhysicsSenderMaxBandwidthBps, 0);
            is_restoring = false;
        }
        else if (!currently_enabled && was_enabled) {
            if (original_value != -1) {
                restore_start_time = std::chrono::steady_clock::now();
                is_restoring = true;
            }
        }
        else if (currently_enabled) {
            // write<int32_t>(base_address + offsets::PhysicsSenderMaxBandwidthBps, 0);
        }

        if (is_restoring && original_value != -1) {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - restore_start_time);
            if (elapsed.count() < 3000) {
                // write<int32_t>(base_address + offsets::PhysicsSenderMaxBandwidthBps, original_value);
            } else {
                is_restoring = false;
                original_value = -1;
            }
        }

        was_enabled = currently_enabled;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
} // namespace hooks
