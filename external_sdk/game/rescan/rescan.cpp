#include "rescan.hpp"

void c_rescan::start_search()
{
    auto current_time = std::chrono::steady_clock::now();
    if (current_time - m_last_check_time < m_check_interval)
        return;

    m_last_check_time = current_time;

    if (!g_main::datamodel)
        return;

    uintptr_t current_place_id = memory->read<uintptr_t>(g_main::datamodel + offsets::PlaceId);

    if (current_place_id != 0 && current_place_id != m_last_place_id)
    {
        util.m_print("Rescan: Place changed, updating pointers...");
        m_last_place_id = current_place_id;

        auto base_address = memory->find_image();
        if (!base_address)
            return;

        uintptr_t fake_datamodel_pointer = memory->read<uintptr_t>(base_address + offsets::FakeDataModelPointer);
        if (!fake_datamodel_pointer)
            return;

        g_main::datamodel = memory->read<uintptr_t>(fake_datamodel_pointer + offsets::FakeDataModelToDataModel);
        g_main::v_engine = memory->read<uintptr_t>(base_address + offsets::VisualEnginePointer);

        auto players_instance = core.find_first_child_class(g_main::datamodel, "Players");
        if (players_instance)
        {
            g_main::localplayer = memory->read<uintptr_t>(players_instance + offsets::LocalPlayer);
        }
    }

    if (g_main::localplayer)
    {
        g_main::localplayer_team = memory->read<uintptr_t>(g_main::localplayer + offsets::Team);
    }
}