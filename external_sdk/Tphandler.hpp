#pragma once
#include "main.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class c_tp_handler {
private:
    std::atomic<bool> running = false;
    std::atomic<bool> needs_reinit = false;
    std::thread monitor_thread;

    uint64_t last_place_id = 0;
    uintptr_t last_datamodel = 0;

public:
    // Quick check if we need to reinitialize
    bool should_reinitialize() {
        // No datamodel = need init
        if (g_main::datamodel == 0) return true;

        // Can't read from datamodel = need init
        uintptr_t test = memory->read<uintptr_t>(g_main::datamodel);
        if (test == 0) return true;

        // Place ID changed = need init (teleported to different game)
        uint64_t current_place = memory->read<uint64_t>(g_main::datamodel + offsets::PlaceId);
        if (current_place != last_place_id && last_place_id != 0) {
            util.m_print("tp_handler: Place changed (%llu -> %llu)", last_place_id, current_place);
            return true;
        }

        // LocalPlayer invalid = need init
        if (g_main::localplayer == 0) return true;
        uintptr_t test_lp = memory->read<uintptr_t>(g_main::localplayer);
        if (test_lp == 0) return true;

        return false;
    }

    void update_cache() {
        if (g_main::datamodel != 0) {
            last_datamodel = g_main::datamodel;
            last_place_id = memory->read<uint64_t>(g_main::datamodel + offsets::PlaceId);
        }
    }

    void monitor_loop() {
        util.m_print("tp_handler: Monitor started");

        while (running) {
            // Sleep longer when game is active and working
            // Sleep shorter when waiting for game
            int sleep_time = (g_main::datamodel != 0) ? 2000 : 500;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

            if (!memory) continue;

            // Check if game process exists
            auto base = memory->find_image();
            if (!base) {
                if (g_main::datamodel != 0) {
                    util.m_print("tp_handler: Game closed");
                    g_main::datamodel = 0;
                    g_main::v_engine = 0;
                    g_main::localplayer = 0;
                    g_main::localplayer_team = 0;
                    last_datamodel = 0;
                    last_place_id = 0;
                }
                continue;
            }

            // Only check if reinit needed
            if (should_reinitialize()) {
                util.m_print("tp_handler: Reinitializing...");
                std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Wait for game to load
                reinitialize_game_pointers();
                update_cache();
            }
        }

        util.m_print("tp_handler: Monitor stopped");
    }

    void start() {
        if (running) return;
        running = true;

        // Initial setup
        reinitialize_game_pointers();
        update_cache();

        monitor_thread = std::thread(&c_tp_handler::monitor_loop, this);
        monitor_thread.detach();

        util.m_print("tp_handler: Started");
    }

    void stop() {
        running = false;
    }

    void force_reinitialize() {
        reinitialize_game_pointers();
        update_cache();
    }

    bool is_ready() const {
        return g_main::datamodel != 0 && g_main::localplayer != 0;
    }
};

inline c_tp_handler tp_handler;