#include "core.hpp"


std::string c_core::read_string ( uintptr_t address )


{


    std::string str;


    char character = 0;


    int char_size = sizeof ( character );
    int offset = 0;

    str.reserve ( 204 );

    while ( offset < 200 ) {
        character = driver.read < char > ( address + offset );

        if ( character == 0 )
            break;

        offset += char_size;
        str.push_back ( character );
    }

    return str;
}

std::string c_core::length_read_string ( uintptr_t string ) 
{
    const auto length = driver.read < int > ( string + offsets::ClassDescriptor );

    if ( length >= 16u ) {
        const auto new_ptr = driver.read < uintptr_t > ( string );
        return read_string ( new_ptr );
    }
    return read_string ( string );
}

std::string c_core::get_instance_name ( uintptr_t instance_address ) 
{
    const auto get_ptr = driver.read < uintptr_t > ( instance_address + offsets::Name );
    return get_ptr ? length_read_string ( get_ptr ) : "???";
}

std::string c_core::get_instance_classname ( uintptr_t instance_address ) 
{
    const auto ptr = driver.read < uintptr_t > ( instance_address + offsets::ClassDescriptor );
    const auto ptr2 = driver.read < uintptr_t > ( ptr + offsets::ClassDescriptorToClassName );
    return ptr2 ? read_string ( ptr2 ) : "???";
}

uintptr_t c_core::find_first_child ( uintptr_t instance_address, const std::string &child_name ) 
{
    if ( !instance_address ) return 0;

    static std::unordered_map < uintptr_t, std::vector < std::pair < uintptr_t, std::string > > > cache;
    static std::unordered_map < uintptr_t, std::chrono::steady_clock::time_point > last_update;

    auto now = std::chrono::steady_clock::now();
    auto &children = cache [ instance_address ];
    auto &update_time = last_update [ instance_address ];

    if ( children.empty() || now - update_time > std::chrono::seconds ( 2 ) ) {
        children.clear();
            auto start = driver.read < uintptr_t > ( instance_address + offsets::Children );
            if ( !start ) return 0;

            auto end = driver.read < uintptr_t > ( start + offsets::ChildrenEnd );
            auto child_array = driver.read < uintptr_t > ( start );
            if ( !child_array || child_array >= end ) return 0;

            for ( uintptr_t current = child_array; current < end; current += 16 ) {
                auto child_instance = driver.read < uintptr_t > ( current );
                if ( !child_instance ) continue;
                children.emplace_back ( child_instance, get_instance_name ( child_instance ) );
            }
        update_time = now;
    }

    for ( const auto &[ child_instance, name ] : children ) {
        if ( name == child_name ) {
            return child_instance;
        }
    }
    return 0;
}

uintptr_t c_core::find_first_child_class( uintptr_t instance_address, const std::string &child_name ) 
{
    if ( !instance_address ) return 0;

    static std::unordered_map < uintptr_t, std::vector < std::pair < uintptr_t, std::string > > > cache;
    static std::unordered_map < uintptr_t, std::chrono::steady_clock::time_point > last_update;

    auto now = std::chrono::steady_clock::now();
    auto &children = cache [ instance_address ];
    auto &update_time = last_update [ instance_address ];

    if ( children.empty() || now - update_time > std::chrono::seconds ( 2 ) ) {
        children.clear();
            auto start = driver.read < uintptr_t > ( instance_address + offsets::Children );
            if ( !start ) return 0;

            auto end = driver.read < uintptr_t > ( start + offsets::ChildrenEnd );
            auto child_array = driver.read < uintptr_t > ( start );
            if ( !child_array || child_array >= end ) return 0;

            for ( uintptr_t current = child_array; current < end; current += 16 ) {
                auto child_instance = driver.read < uintptr_t > ( current );
                if ( !child_instance ) continue;
                children.emplace_back ( child_instance, get_instance_classname ( child_instance ) );
            }
        update_time = now;
    }

    for ( const auto &[ child_instance, name ] : children ) {
        if ( name == child_name ) {
            return child_instance;
        }
    }
    return 0;
}

uintptr_t c_core::get_model_instance ( uintptr_t instance_address ) 
{
    return driver.read < uintptr_t > ( instance_address + offsets::ModelInstance );
}

std::vector < uintptr_t > c_core::children ( uintptr_t instance_address ) 
{
    std::vector < uintptr_t > container;
    if ( !instance_address ) return container;

    auto start = driver.read < uintptr_t > ( instance_address + offsets::Children );
    if ( !start ) return container;

    auto end = driver.read < uintptr_t > ( start + offsets::ChildrenEnd );
    for ( auto instances = driver.read < uintptr_t > ( start ); instances != end; instances += 16 ) {
        container.emplace_back ( driver.read < uintptr_t > ( instances ) );
    }
    return container;
}

std::vector < uintptr_t > c_core::get_players ( uintptr_t datamodel_address ) 
{
    std::vector < uintptr_t > player_instances;
    auto players_instance = find_first_child_class ( datamodel_address, "Players" );
    if ( !players_instance ) return player_instances;

    auto children_addresses = children ( players_instance );
    player_instances.reserve ( children_addresses.size() );

    for ( const auto &child : children_addresses ) {
        if ( get_instance_classname ( child ) == "Player" ) {
            player_instances.push_back ( child );
        }
    }
    return player_instances;
}

uintptr_t c_core::get_local_humanoid()
{
    if (!g_main::datamodel || !g_main::localplayer)
        return 0;

    uintptr_t workspace = find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace)
        return 0;

    uintptr_t local_player_character_model = find_first_child(workspace, get_instance_name(g_main::localplayer));
    if (!local_player_character_model)
        return 0;

    return find_first_child_class(local_player_character_model, "Humanoid");
}

matrix c_core::cframe_to_matrix(const CFrame& cframe)
{
    matrix m;

    // Rotation components (transposed for inverse)
    m.m[0][0] = cframe.R00; m.m[0][1] = cframe.R10; m.m[0][2] = cframe.R20; m.m[0][3] = 0.0f;
    m.m[1][0] = cframe.R01; m.m[1][1] = cframe.R11; m.m[1][2] = cframe.R21; m.m[1][3] = 0.0f;
    m.m[2][0] = cframe.R02; m.m[2][1] = cframe.R12; m.m[2][2] = cframe.R22; m.m[2][3] = 0.0f;

    // Translation components (inverse)
    m.m[3][0] = -(cframe.R00 * cframe.Position.x + cframe.R10 * cframe.Position.y + cframe.R20 * cframe.Position.z);
    m.m[3][1] = -(cframe.R01 * cframe.Position.x + cframe.R11 * cframe.Position.y + cframe.R21 * cframe.Position.z);
    m.m[3][2] = -(cframe.R02 * cframe.Position.x + cframe.R12 * cframe.Position.y + cframe.R22 * cframe.Position.z);
    m.m[3][3] = 1.0f;

    return m;
}

bool c_core::world_to_screen( const vector& world_pos, vector2d& screen_pos, const matrix& view_matrix )
{
    if ( !this->screen_height || !this->screen_width )
    {
        this->screen_width = GetSystemMetrics( SM_CXSCREEN );
        this->screen_height = GetSystemMetrics( SM_CYSCREEN );
    }
    float w = view_matrix.m [ 3 ] [ 0 ] * world_pos.x +
              view_matrix.m [ 3 ] [ 1 ] * world_pos.y +
              view_matrix.m [ 3 ] [ 2 ] * world_pos.z +
              view_matrix.m [ 3 ] [ 3 ];

    if ( w <= 0.001f ) return false;

    float inv_w = 1.0f / w;

    screen_pos.x = ( view_matrix.m [ 0 ] [ 0 ] * world_pos.x +
                    view_matrix.m [ 0 ] [ 1 ] * world_pos.y +
                    view_matrix.m [ 0 ] [ 2 ] * world_pos.z +
                    view_matrix.m [ 0 ] [ 3 ] ) * inv_w;

    screen_pos.y = ( view_matrix.m [ 1 ] [ 0 ] * world_pos.x +
                    view_matrix.m [ 1 ] [ 1 ] * world_pos.y +
                    view_matrix.m [ 1 ] [ 2 ] * world_pos.z +
                    view_matrix.m [ 1 ] [ 3 ] ) * inv_w;

    screen_pos.x = ( this->screen_width * 0.5f ) * ( screen_pos.x + 1.0f );
    screen_pos.y = ( this->screen_height * 0.5f ) * ( 1.0f - screen_pos.y );

    return true;
}