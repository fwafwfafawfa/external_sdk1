#pragma once
#include "main.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class c_tp_handler {
private:
    std::atomic<bool> running = false;
    std::thread monitor_thread;

    uint64_t last_place_id = 0;
    uint64_t last_game_id = 0;
    uintptr_t last_base_address = 0;

public:
    bool should_reinitialize() {
        if (!memory) return true;

        auto base = memory->find_image();
        if (!base) return true;

        if (last_base_address != 0 && base != last_base_address) {
            util.m_print("tp_handler: Base address changed (0x%llX -> 0x%llX)", last_base_address, base);
            return true;
        }

        if (g_main::datamodel == 0) return true;

        uintptr_t test = memory->read<uintptr_t>(g_main::datamodel);
        if (test == 0 || test < 0x10000) return true;

        uint64_t current_place = memory->read<uint64_t>(g_main::datamodel + offsets::PlaceId);
        uint64_t current_game = memory->read<uint64_t>(g_main::datamodel + offsets::GameId);

        if (current_place != last_place_id && last_place_id != 0) {
            util.m_print("tp_handler: Teleported (%llu -> %llu)", last_place_id, current_place);
            return true;
        }

        if (current_game != last_game_id && last_game_id != 0) {
            util.m_print("tp_handler: Game changed (%llu -> %llu)", last_game_id, current_game);
            return true;
        }

        if (g_main::localplayer == 0) return true;
        uintptr_t test_lp = memory->read<uintptr_t>(g_main::localplayer);
        if (test_lp == 0 || test_lp < 0x10000) return true;

        return false;
    }

    void update_cache() {
        if (memory) {
            last_base_address = memory->find_image();
        }

        if (g_main::datamodel != 0) {
            last_place_id = memory->read<uint64_t>(g_main::datamodel + offsets::PlaceId);
            last_game_id = memory->read<uint64_t>(g_main::datamodel + offsets::GameId);
        }
    }

    void monitor_loop() {
        util.m_print("tp_handler: Monitor started");

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            if (!memory) continue;

            auto base = memory->find_image();
            if (!base) {
                if (g_main::datamodel != 0) {
                    util.m_print("tp_handler: Game closed, waiting for restart...");
                    g_main::datamodel = 0;
                    g_main::v_engine = 0;
                    g_main::localplayer = 0;
                    g_main::localplayer_team = 0;
                    last_place_id = 0;
                    last_game_id = 0;
                    last_base_address = 0;
                }

                delete memory;
                memory = new c_memory("RobloxPlayerBeta.exe");
                if (!memory || !memory->find_image()) {
                    continue;
                }

                util.m_print("tp_handler: Detected new Roblox process");
            }

            if (g_main::datamodel == 0 || should_reinitialize()) {
                util.m_print("tp_handler: Detected change, reinitializing...");
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));

                reinitialize_game_pointers();

                if (g_main::datamodel != 0) {
                    update_cache();
                    util.m_print("tp_handler: Reinitialized successfully");
                }
            }
        }

        util.m_print("tp_handler: Monitor stopped");
    }


    void start() {
        if (running) return;
        running = true;

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
        return g_main::datamodel != 0 && g_main::localplayer != 0 && memory && memory->find_image() != 0;
    }
};

inline c_tp_handler tp_handler;
