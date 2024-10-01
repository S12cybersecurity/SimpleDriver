#include <Windows.h>
#include <iostream>

int main() {
    HANDLE hDevice = CreateFile(
        L"\\\\.\\MyDevice", // Device path
        GENERIC_READ,       // Read access
        0,                  // No sharing
        NULL,               // No security attributes
        OPEN_EXISTING,      // Opens existing device
        0,                  // No special flags
        NULL                // No template file
    );


    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening device: " << GetLastError() << std::endl;
        return 1;
    }

    char buffer[128];
    DWORD bytesRead;

    BOOL result = ReadFile(
        hDevice,
        buffer,
        sizeof(buffer),
        &bytesRead,
        NULL
    );

    if (!result) {
        std::cerr << "Error reading from device: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return 1;
    }

    std::cout << "Read " << bytesRead << " bytes from the device: " << buffer << std::endl;

    CloseHandle(hDevice);
    return 0;
}
