#pragma once

#define MEDREADWYY CTL_CODE( FILE_DEVICE_UNKNOWN, 0x07D, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define MEDBAB3 CTL_CODE( FILE_DEVICE_UNKNOWN, 0x09A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define SCODE_SECURITY 0x85b3e20

#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>

struct read_write {
    int32_t security;
    int32_t process_id;
    uint64_t address;
    uint64_t buffer;
    uint64_t size;
    bool write;
};

struct base_address {
    int32_t security;
    int32_t process_id;
    uint64_t address;
};

class c_driver {
public:
    c_driver( );
    ~c_driver( );

    bool find_driver( );
    void read_physical( void* address, void* buffer, DWORD size );
    void write_physical( void* address, void* buffer, DWORD size );
    uintptr_t find_image( );
    int32_t find_process( const char* process_name );

    template < typename T >
    T read( uint64_t address ) {
        T buffer{};
        read_physical( reinterpret_cast< void* >( address ), &buffer, sizeof( T ) );
        return buffer;
    }

    template < typename T >
    void write( uint64_t address, const T& buffer ) {
        write_physical( reinterpret_cast< void* >( address ), const_cast< T* >( &buffer ), sizeof( T ) );
    }

private:
    HANDLE driver_handle;
    int32_t process_id;
};

inline c_driver driver;
