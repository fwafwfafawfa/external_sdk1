#include "driver.hpp"
#include <stdio.h>

c_driver::c_driver( ):
    driver_handle( INVALID_HANDLE_VALUE ), process_id( 0 )
{
}

c_driver::~c_driver( ) {
    if ( driver_handle != INVALID_HANDLE_VALUE ) {
        CloseHandle( driver_handle );
    }
}

bool c_driver::find_driver( ) {
    driver_handle = CreateFileW( L"\\\\.\\MedusaNewCom", GENERIC_READ | GENERIC_WRITE, 
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr );
    return driver_handle != nullptr && driver_handle != INVALID_HANDLE_VALUE;
}

void c_driver::read_physical(void* address, void* buffer, DWORD size) {
    read_write args{};
    args.security = SCODE_SECURITY;
    args.address = reinterpret_cast<uint64_t>(address);
    args.buffer = 0; // Not used for input
    args.size = size;
    args.process_id = process_id;
    args.write = false;
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(driver_handle, MEDREADWYY, &args, sizeof(args), buffer, size, &bytesReturned, nullptr)) {
        printf("\033[91m[error] \033[0mDeviceIoControl failed in read_physical. GetLastError() = %d\n", GetLastError());
    }
}

void c_driver::write_physical( void* address, void* buffer, DWORD size ) {
    read_write args{};
    args.security = SCODE_SECURITY;
    args.address = reinterpret_cast< uint64_t >( address );
    args.buffer = reinterpret_cast< uint64_t >( buffer );
    args.size = size;
    args.process_id = process_id;
    args.write = true;

    if ( !DeviceIoControl( driver_handle, MEDREADWYY, &args, sizeof( args ), nullptr, 0, nullptr, nullptr ) )
    {
        printf( "\033[91m[error] \033[0mDeviceIoControl failed in write_physical. GetLastError() = %d\n", GetLastError() );
    }
}

uintptr_t c_driver::find_image( ) {
    base_address args{};
    args.security = SCODE_SECURITY;
    args.process_id = process_id;
    args.address = 0;
    DWORD bytesReturned = 0;

    if ( !DeviceIoControl( driver_handle, MEDBAB3, &args, sizeof( args ), &args, sizeof( args ), &bytesReturned, nullptr ) )
    {
        printf( "\033[91m[error] \033[0mDeviceIoControl failed in find_image. GetLastError() = %d\n", GetLastError() );
    }
    else
    {
        printf( "\033[92m[success] \033[0mDeviceIoControl in find_image succeeded. Bytes returned: %d\n", bytesReturned );
    }
    
    return static_cast<uintptr_t>(args.address);
}

int32_t c_driver::find_process( const char* process_name ) {
    PROCESSENTRY32 process_entry{};
    process_entry.dwSize = sizeof( PROCESSENTRY32 );

    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if ( snapshot == INVALID_HANDLE_VALUE ) {
        return 0;
    }

    if ( Process32First( snapshot, &process_entry ) ) {
        do {
            if ( _stricmp( process_entry.szExeFile, process_name ) == 0 ) {
                process_id = process_entry.th32ProcessID;
                CloseHandle( snapshot );
                return process_id;
            }
        } while ( Process32Next( snapshot, &process_entry ) );
    }

    CloseHandle( snapshot );
    return 0;
}
