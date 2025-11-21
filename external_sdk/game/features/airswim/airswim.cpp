#include "airswim.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../addons/kernel/memory.hpp"
#include "../../core.hpp"

void c_airswim::run( )
{
    if ( !vars::airswim::toggled )
        return;

    uintptr_t local_humanoid = core.get_local_humanoid( );
    if ( !local_humanoid )
        return;

    // HumanoidStateType::Swimming is enum 4
    // We write this to the humanoid's state controller to enable swimming physics in the air.
    uintptr_t humanoid_state = memory->read<uintptr_t>( local_humanoid + offsets::HumanoidState );
    if ( humanoid_state )
        memory->write<uintptr_t>( humanoid_state + offsets::HumanoidStateId, 4 );
}