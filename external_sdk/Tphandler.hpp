#pragma once
#include "main.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class c_tp_handler {
private:
    std::atomic<bool> running = false;
    std::thread monitor_thread;

    uint64_t last_job_id = 0;

    std::atomic<bool> force_rescan_flag = false;
    std::atomic<bool> is_hopping = false;

    // Safe memory read that won't crash
    template<typename T>
    T safe_read(uintptr_t address, T default_value = T()) {
        if (!memory) return default_value;
        if (!address || address < 0x10000) return default_value;

        try {
            return memory->read<T>(address);
        }
        catch (...) {
            return default_value;
        }
    }

    // Check if memory object is valid
    bool is_memory_valid() {
        if (!memory) return false;

        try {
            auto base = memory->find_image();
            return base != 0;
        }
        catch (...) {
            return false;
        }
    }

    // Safe delete memory
    void safe_delete_memory() {
        if (memory) {
            try {
                delete memory;
            }
            catch (...) {}
            memory = nullptr;
        }
    }

public:
    void request_rescan() {
        is_hopping = true;
        force_rescan_flag = true;
        util.m_print("tp_handler: Rescan requested");
    }

    bool is_ready() const {
        return g_main::datamodel != 0 &&
            g_main::localplayer != 0 &&
            memory != nullptr &&
            !is_hopping;
    }

    void update_cache() {
        if (g_main::datamodel != 0) {
            last_job_id = safe_read<uint64_t>(g_main::datamodel + offsets::JobId, 0);
        }
    }

    void monitor_loop() {
        util.m_print("tp_handler: Monitor started");

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            try {
                // ============================================
                // 1. HOPPING MODE - Full Reset
                // ============================================
                if (is_hopping) {
                    util.m_print("tp_handler: Hop detected, resetting...");

                    // Clear everything
                    safe_delete_memory();
                    g_main::datamodel = 0;
                    g_main::localplayer = 0;
                    g_main::v_engine = 0;
                    last_job_id = 0;

                    // Wait for window to close (max 10 sec)
                    util.m_print("tp_handler: Waiting for old window to close...");
                    for (int i = 0; i < 20 && FindWindowA(NULL, "Roblox"); i++) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }

                    // Wait for new window (max 30 sec)
                    util.m_print("tp_handler: Waiting for new window...");
                    for (int i = 0; i < 60 && !FindWindowA(NULL, "Roblox"); i++) {
                        if (!running) break;
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }

                    if (!running) break;

                    // Wait for game to initialize
                    util.m_print("tp_handler: Window found, waiting for init...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(8000));

                    is_hopping = false;
                    // Don't create memory here, let the normal flow handle it
                }

                // ============================================
                // 2. CHECK WINDOW EXISTS
                // ============================================
                HWND hwnd = FindWindowA(NULL, "Roblox");
                if (!hwnd) {
                    // No window = no game
                    if (g_main::datamodel != 0) {
                        util.m_print("tp_handler: Window gone, clearing...");
                        safe_delete_memory();
                        g_main::datamodel = 0;
                        g_main::localplayer = 0;
                    }
                    continue;
                }

                // ============================================
                // 3. CHECK/CREATE MEMORY OBJECT
                // ============================================
                if (!is_memory_valid()) {
                    safe_delete_memory();

                    util.m_print("tp_handler: Creating memory object...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                    try {
                        memory = new c_memory("RobloxPlayerBeta.exe");
                    }
                    catch (...) {
                        memory = nullptr;
                        continue;
                    }

                    if (!is_memory_valid()) {
                        safe_delete_memory();
                        continue;
                    }

                    util.m_print("tp_handler: Memory attached");
                    force_rescan_flag = true;
                }

                // ============================================
                // 4. CHECK IF POINTERS NEED REFRESH
                // ============================================
                bool needs_refresh = false;

                if (force_rescan_flag) {
                    needs_refresh = true;
                    force_rescan_flag = false;
                }
                else if (g_main::datamodel == 0 || g_main::localplayer == 0) {
                    needs_refresh = true;
                }
                else {
                    // Validate existing pointers
                    uintptr_t test = safe_read<uintptr_t>(g_main::datamodel, 0);
                    if (test < 0x10000) {
                        needs_refresh = true;
                    }

                    // Check JobId change (server hop without our knowledge)
                    uint64_t current_job = safe_read<uint64_t>(g_main::datamodel + offsets::JobId, 0);
                    if (current_job != 0 && last_job_id != 0 && current_job != last_job_id) {
                        util.m_print("tp_handler: JobId changed, refreshing...");
                        needs_refresh = true;
                    }
                }

                // ============================================
                // 5. REFRESH POINTERS
                // ============================================
                if (needs_refresh) {
                    util.m_print("tp_handler: Refreshing pointers...");

                    bool success = false;
                    for (int attempt = 0; attempt < 15; attempt++) {
                        try {
                            reinitialize_game_pointers();
                        }
                        catch (...) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            continue;
                        }

                        if (g_main::datamodel != 0 && g_main::localplayer != 0) {
                            success = true;
                            break;
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }

                    if (success) {
                        update_cache();
                        util.m_print("tp_handler: Ready!");
                    }
                    else {
                        util.m_print("tp_handler: Failed to get pointers");
                    }
                }

            }
            catch (...) {
                util.m_print("tp_handler: Exception caught, resetting...");
                safe_delete_memory();
                g_main::datamodel = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        }

        util.m_print("tp_handler: Monitor stopped");
    }

    void start() {
        if (running) return;
        running = true;

        try {
            reinitialize_game_pointers();
            update_cache();
        }
        catch (...) {}

        monitor_thread = std::thread(&c_tp_handler::monitor_loop, this);
        monitor_thread.detach();

        util.m_print("tp_handler: Started");
    }

    void stop() {
        running = false;
    }
};

inline c_tp_handler tp_handler;