#include <ntddk.h>
#include <wdf.h>

// Handler para solicitudes de lectura
NTSTATUS IrpReadHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    // Completar la solicitud de lectura sin hacer nada especial
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0; // No se leen bytes
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IRP_MJ_READ handled\n");
    return STATUS_SUCCESS;
}

// Handler para solicitudes de creación
NTSTATUS IrpCreateHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    // Completa la solicitud de creación
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IRP_MJ_CREATE handled\n");
    return STATUS_SUCCESS;
}

// Función de descarga del controlador
VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\MyDevice");
    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);
}

// Función de entrada del controlador
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    UNICODE_STRING devName;
    RtlInitUnicodeString(&devName, L"\\Device\\MyDevice");
    PDEVICE_OBJECT DeviceObject;
    NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Device created, status: %x\n", status);

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to create device: %x\n", status);
        return status;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Driver loaded\n");

    // Manejo del enlace simbólico
    UNICODE_STRING symLink;
    RtlInitUnicodeString(&symLink, L"\\??\\MyDevice");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Attempting to delete existing symbolic link.\n");

    NTSTATUS deleteStatus = IoDeleteSymbolicLink(&symLink);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Delete symbolic link status: %x\n", deleteStatus);

    // Crear el nuevo enlace simbólico
    status = IoCreateSymbolicLink(&symLink, &devName);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "New symbolic link status: %x\n", status);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return status;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Symbolic link created successfully.\n");

    // Asignar manejadores
    DriverObject->DriverUnload = UnloadDriver;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateHandler; // Agregar manejador de creación
    DriverObject->MajorFunction[IRP_MJ_READ] = IrpReadHandler;     // Asignar manejador de lectura

    return STATUS_SUCCESS;
}
