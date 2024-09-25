#include <ntddk.h>
#include <wdf.h>

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

VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
	// Code for the unload driver situation
	// Delete the device object and symbolic link
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\MyDevice");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

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

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Driver loaded\n");

	// Create a symbolic link
	UNICODE_STRING symLink;
	RtlInitUnicodeString(&symLink, L"\\??\\MyDevice");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		return status;
	}
   
    DriverObject->DriverUnload = UnloadDriver;  
    DriverObject->MajorFunction[IRP_MJ_READ] = IrpReadHandler;

    return STATUS_SUCCESS;
}