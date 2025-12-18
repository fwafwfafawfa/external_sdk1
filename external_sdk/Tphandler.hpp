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

public:
    void request_rescan() {
        is_hopping = true;
        force_rescan_flag = true;
        util.m_print("tp_handler: Rescan requested");
    }

    bool is_ready() const {
        if (is_hopping) return false;
        if (!memory) return false;
        if (g_main::datamodel == 0) return false;
        if (g_main::localplayer == 0) return false;
        if (g_main::v_engine == 0) return false;
        return true;
    }

    void update_cache() {
        if (g_main::datamodel != 0) {
            last_job_id = memory->read<uint64_t>(g_main::datamodel + offsets::JobId);
        }
    }

    void monitor_loop() {
        util.m_print("tp_handler: Monitor started");

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            try {
                // ====== HOPPING MODE ======
                if (is_hopping) {
                    util.m_print("tp_handler: [HOP] Hopping mode active...");

                    // Clear memory
                    if (memory) {
                        util.m_print("tp_handler: [HOP] Deleting old memory object...");
                        delete memory;
                        memory = nullptr;
                    }
                    g_main::datamodel = 0;
                    g_main::localplayer = 0;
                    g_main::v_engine = 0;
                    last_job_id = 0;

                    // Wait for Roblox to close
                    util.m_print("tp_handler: [HOP] Waiting for Roblox to close...");
                    while (FindWindowA(NULL, "Roblox")) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                    util.m_print("tp_handler: [HOP] Roblox closed!");

                    // SAFE MODE OFF HERE
                    g_safe_mode = false;
                    util.m_print("tp_handler: [HOP] Safe mode disabled");

                    // Wait for new window
                    util.m_print("tp_handler: [HOP] Waiting for new Roblox window...");
                    int timeout = 0;
                    while (!FindWindowA(NULL, "Roblox") && running && timeout < 120) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        timeout++;
                    }

                    if (!running) break;

                    if (timeout >= 120) {
                        util.m_print("tp_handler: [HOP] Timeout waiting for window!");
                        is_hopping = false;
                        continue;
                    }

                    util.m_print("tp_handler: [HOP] New window found! Waiting for game to load...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(8));  // 15 seconds for BSS

                    is_hopping = false;
                    force_rescan_flag = true;
                    util.m_print("tp_handler: [HOP] Ready to reattach!");
                }

                // ====== CHECK WINDOW ======
                HWND hwnd = FindWindowA(NULL, "Roblox");
                if (!hwnd) {
                    if (g_main::datamodel != 0) {
                        util.m_print("tp_handler: Window gone, clearing pointers");
                        if (memory) { delete memory; memory = nullptr; }
                        g_main::datamodel = 0;
                        g_main::localplayer = 0;
                        g_main::v_engine = 0;
                    }
                    continue;
                }

                // ====== CREATE MEMORY ======
                if (!memory) {
                    util.m_print("tp_handler: No memory object, creating...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                    memory = new c_memory("RobloxPlayerBeta.exe");

                    if (!memory) {
                        util.m_print("tp_handler: Failed to create memory object!");
                        continue;
                    }

                    DWORD pid = memory->get_pid();  // Add get_pid() if you don't have it
                    util.m_print("tp_handler: Attached to PID: %d", pid);

                    uintptr_t base = memory->find_image();
                    util.m_print("tp_handler: Base address: 0x%llX", base);

                    if (!base) {
                        util.m_print("tp_handler: Failed to find base address!");
                        delete memory;
                        memory = nullptr;
                        continue;
                    }

                    util.m_print("tp_handler: Memory attached successfully!");
                    force_rescan_flag = true;
                }

                // ====== CHECK MEMORY VALID ======
                if (!memory->find_image()) {
                    util.m_print("tp_handler: Memory invalid!");
                    delete memory;
                    memory = nullptr;
                    continue;
                }

                // ====== REINITIALIZE POINTERS ======
                bool needs_init = force_rescan_flag ||
                    g_main::datamodel == 0 ||
                    g_main::localplayer == 0;

                if (needs_init) {
                    util.m_print("tp_handler: Initializing pointers...");
                    force_rescan_flag = false;

                    for (int attempt = 0; attempt < 10; attempt++) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                        reinitialize_game_pointers();

                        if (g_main::datamodel != 0 && g_main::localplayer != 0) {
                            update_cache();
                            vars::bss::is_hopping = false;
                            util.m_print("tp_handler: SUCCESS! Pointers found.");
                            break;
                        }

                        util.m_print("tp_handler: Attempt %d/10 failed...", attempt + 1);
                    }

                    if (g_main::datamodel == 0) {
                        util.m_print("tp_handler: Failed after 10 attempts!");
                    }

                    if (g_main::datamodel == 0) {
                        util.m_print("tp_handler: Failed after 30 attempts!");
                        // Re-enable safe mode if we failed
                        g_safe_mode = true;
                    }
                }

            }
            catch (...) {
                util.m_print("tp_handler: EXCEPTION! Resetting...");
                if (memory) { delete memory; memory = nullptr; }
                g_main::datamodel = 0;
                g_main::localplayer = 0;
                g_main::v_engine = 0;
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
};

inline c_tp_handler tp_handler;