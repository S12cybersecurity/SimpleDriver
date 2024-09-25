# SimpleDriver
Simple C++ Windows Kernel Driver 

### Create Essential Files

You can create essential files to start implementing functionality. Here are the key components you might need:

- **Driver.cpp**: This file will contain your driver code, including the `DriverEntry` function and any other major functions for handling I/O requests.
- **Device.cpp**: This file is for device-specific functions, such as creating and managing the device object.
- **IOCTL.h**: Define the I/O Control Codes (IOCTLs) your driver will use for communication with user-mode applications.
### Implement Driver Entry Point

You need to implement the `DriverEntry` function in `Driver.cpp`. This function is the main entry point for your driver:
```cpp
#include <ntddk.h>
#include <wdf.h>

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    DriverObject->DriverUnload = UnloadDriver;  
    DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateHandler; 
    DriverObject->MajorFunction[IRP_MJ_READ] = IrpReadHandler;

    return STATUS_SUCCESS;
}
```
### Define I/O Request Handlers

In `Driver.cpp`, implement handlers for different I/O requests like `IRP_MJ_READ`, `IRP_MJ_WRITE`, etc. For example, for a read operation:
```cpp
NTSTATUS IrpReadHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) { 
	// Maneja la solicitud de lectura 
	// Obtén la ubicación de la pila, procesa los datos y completa el IRP 
	return STATUS_SUCCESS; 
} 

VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
	// Código para liberar recursos cuando el driver se descarga 
}
```

### Create a Device Object
You will need to create a device object that your driver will manage. This typically happens in the `DriverEntry` function.
```cpp
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);
    // Create a device object
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevice");
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		return status;
	}
...
```

### Create SymLink
```cpp
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    // Create a device object
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevice");
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	// Create a symbolic lin
	UNICODE_STRING symLink;
	RtlInitUnicodeString(&symLink, L"\\??\\MyDevice");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		return status;
	}
	...
```

### Create Unload Routine
```cpp
VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
	// Code for the unload driver situation
	// Delete the device object and symbolic link
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\MyDevice");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}
```

### Create Dispatcher Routines
Example:
```cpp

// In DriverEntry
DriverObject->MajorFunction[IRP_MJ_READ] = IrpReadHandler;


NTSTATUS IrpReadHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	// Code for the read IRP
	// Get the buffer from the IRP
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	switch (stack->MajorFunction) {
	case IRP_MJ_READ:
		// Read the buffer
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IRP_MJ_READ\n");
		break;
	}
    return STATUS_SUCCESS;
}
