#include "main.hpp"
#include "handlers/themes/theme.hpp"

void reinitialize_game_pointers()
{
    util.m_print("reinitialize_game_pointers: Starting...");

    const int max_attempts = 10;
    for (int attempt = 1; attempt <= max_attempts; ++attempt) {
        auto base_address = memory->find_image();
        if (!base_address) {
            util.m_print("reinitialize_game_pointers: Attempt %d: no base address, retrying...", attempt);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        uintptr_t fake_datamodel_pointer = memory->read<uintptr_t>(base_address + offsets::FakeDataModelPointer);
        uintptr_t datamodel = fake_datamodel_pointer ? memory->read<uintptr_t>(fake_datamodel_pointer + offsets::FakeDataModelToDataModel) : 0;
        uintptr_t v_engine = memory->read<uintptr_t>(base_address + offsets::VisualEnginePointer);
        uintptr_t players_instance = datamodel ? core.find_first_child_class(datamodel, "Players") : 0;
        uintptr_t localplayer = players_instance ? memory->read<uintptr_t>(players_instance + offsets::LocalPlayer) : 0;
        uintptr_t localplayer_team = localplayer ? memory->read<uintptr_t>(localplayer + offsets::Team) : 0;

        if (datamodel && v_engine && players_instance && localplayer && localplayer_team) {
            g_main::datamodel = datamodel;
            g_main::v_engine = v_engine;
            g_main::localplayer = localplayer;
            g_main::localplayer_team = localplayer_team;

            util.m_print("reinitialize_game_pointers: Success on attempt %d", attempt);
            util.m_print("Base: 0x%llX FakeDataModel: 0x%llX Datamodel: 0x%llX VisualEngine: 0x%llX Players: 0x%llX LocalPlayer: 0x%llX Team: 0x%llX",
                         base_address, fake_datamodel_pointer, datamodel, v_engine, players_instance, localplayer, localplayer_team);
            auto place_id = memory->read<uintptr_t>(g_main::datamodel + offsets::PlaceId);
            util.m_print("reinitialize_game_pointers: Place ID: %llu", place_id);
            return;
        }

        util.m_print("reinitialize_game_pointers: Attempt %d: incomplete, retrying...", attempt);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    util.m_print("reinitialize_game_pointers: Failed after %d attempts.", max_attempts);
}

int main( )
{
    memory = new c_memory( "RobloxPlayerBeta.exe" );

    if ( !memory )
    {
        util.m_print( "Failed to find Roblox, run the game." );
        std::cin.get( );
        return 0;
    }

    // reinitialize_game_pointers(); // This is now handled by the rescan thread in the main loop

    config.load("settings.ini"); // Load settings on startup

    overlay.start( );
}
