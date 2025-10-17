#include "main.hpp"

void reinitialize_game_pointers()
{
    auto base_address = driver.find_image();
    if (!base_address) {
        util.m_print("Failed to find base address.");
        return;
    }

    uintptr_t fake_datamodel_pointer = driver.read<uintptr_t>(base_address + offsets::FakeDataModelPointer);
    if (!fake_datamodel_pointer) {
        util.m_print("Failed to find fake datamodel pointer.");
        return;
    }

    g_main::datamodel = driver.read<uintptr_t>(fake_datamodel_pointer + offsets::FakeDataModelToDataModel);
    if (!g_main::datamodel) {
        util.m_print("Failed to find datamodel.");
        return;
    }

    g_main::v_engine = driver.read<uintptr_t>(base_address + offsets::VisualEnginePointer);
    if (!g_main::v_engine) {
        util.m_print("Failed to find visual engine.");
        return;
    }

    auto players_instance = core.find_first_child_class(g_main::datamodel, "Players");
    if (!players_instance) {
        util.m_print("Failed to find Players instance.");
        return;
    }

    g_main::localplayer = driver.read<uintptr_t>(players_instance + offsets::LocalPlayer);
    if (!g_main::localplayer) {
        util.m_print("Failed to find local player.");
        return;
    }

    g_main::localplayer_team = driver.read<uintptr_t>(g_main::localplayer + offsets::Team);
    if (!g_main::localplayer_team) {
        util.m_print("Failed to find local player team.");
        return;
    }

    auto place_id = driver.read<uintptr_t>(g_main::datamodel + offsets::PlaceId);

    util.m_print("Addresses:");
    util.m_print("Base Address: 0x%llX", base_address);
    util.m_print("DataModel: 0x%llX", g_main::datamodel);
    util.m_print("Visual Engine: 0x%llX", g_main::v_engine);
    util.m_print("Local Player: 0x%llX", g_main::localplayer);
    util.m_print("Local Player Team: 0x%llX", g_main::localplayer_team);
    util.m_print("Players Instance: 0x%llX", players_instance);
    util.m_print("Place ID: %llu", place_id);
}

int main( )
{
    auto process = driver.find_process( "RobloxPlayerBeta.exe" );

    if ( !driver.find_driver( ) )
    {
        util.m_print( "Failed to find driver, load it using a mapper or try restarting your PC." );
        std::cin.get( );
        return 0;
    }

    if ( !process )
    {
        util.m_print( "Failed to find Roblox, run the game." );
        std::cin.get( );
        return 0;
    }

    util.m_print( "Process ID: %d", process );

    reinitialize_game_pointers();

    config.load("settings.ini"); // Load settings on startup

    overlay.start( );
}