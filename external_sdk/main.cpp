#include "main.hpp"
#include "handlers/themes/theme.hpp"
#include "tphandler.hpp" // Add this include

void reinitialize_game_pointers()
{
    util.m_print("reinitialize_game_pointers: Starting...");

    const int max_attempts = 10;
    for (int attempt = 1; attempt <= max_attempts; ++attempt) {

        if (!memory) {
            util.m_print("reinitialize_game_pointers: No memory object!");
            return;
        }

        auto base_address = memory->find_image();
        if (!base_address) {
            util.m_print("reinitialize_game_pointers: Attempt %d: no base address", attempt);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        uintptr_t fake_datamodel_pointer = memory->read<uintptr_t>(base_address + offsets::FakeDataModelPointer);
        util.m_print("  FakeDataModel: 0x%llX", fake_datamodel_pointer);

        uintptr_t datamodel = 0;
        if (fake_datamodel_pointer) {
            datamodel = memory->read<uintptr_t>(fake_datamodel_pointer + offsets::FakeDataModelToDataModel);
        }
        util.m_print("  DataModel: 0x%llX", datamodel);

        uintptr_t v_engine = memory->read<uintptr_t>(base_address + offsets::VisualEnginePointer);
        util.m_print("  VisualEngine: 0x%llX", v_engine);

        uintptr_t players_instance = 0;
        if (datamodel) {
            players_instance = core.find_first_child_class(datamodel, "Players");
        }
        util.m_print("  Players: 0x%llX", players_instance);

        uintptr_t localplayer = 0;
        if (players_instance) {
            localplayer = memory->read<uintptr_t>(players_instance + offsets::LocalPlayer);
        }
        util.m_print("  LocalPlayer: 0x%llX", localplayer);

        if (datamodel && v_engine && players_instance && localplayer) {
            uintptr_t localplayer_team = memory->read<uintptr_t>(localplayer + offsets::Player::Team);

            g_main::datamodel = datamodel;
            g_main::v_engine = v_engine;
            g_main::localplayer = localplayer;
            g_main::localplayer_team = localplayer_team;

            util.m_print("reinitialize_game_pointers: SUCCESS on attempt %d", attempt);
            return;
        }

        util.m_print("reinitialize_game_pointers: Attempt %d: incomplete", attempt);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    util.m_print("reinitialize_game_pointers: Failed after %d attempts", max_attempts);
}

int main()
{
    memory = new c_memory("RobloxPlayerBeta.exe");

    if (!memory)
    {
        util.m_print("Failed to find Roblox, run the game.");
        std::cin.get();
        return 0;
    }

    config.load("settings.ini");

    // Start the TP handler (monitors game state and reinitializes when needed)
    tp_handler.start();

    overlay.start();
}