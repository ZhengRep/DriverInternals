#include "HelloDDK.h"
#include "..\HelloWDM\HelloWDM.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN UNICODE_STRING pRegisterPath)
{
    UNREFERENCED_PARAMETER(pRegisterPath);

    DbgPrint("HelloDDK, DbgPrint\n");
    KdPrint(("HelloDDK:Enter DriverEntry.\n"));
    NTSTATUS status;

    //Dispatch
    pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatch;
    pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatch;
    pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatch;
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatch;
    //Unload
    pDriverObject->DriverUnload = HelloDDKUnload;

    //Create device\n
    status = CreateDevice(pDriverObject);

    KdPrint(("HelloDDK:DriverEntry End.\n"));
    return status;
}


#pragma PAGEDCODE
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject)
{
    KdPrint(("Enter HelloDDKUnload\n"));
    PDEVICE_OBJECT pNextDeviceObject = pDriverObject->DeviceObject;

    while (pNextDeviceObject != NULL)
    {
        PDEVICE_EXTENSION pDeviceExtObject = (PDEVICE_EXTENSION)pNextDeviceObject->DeviceExtension;
        //Delete SymLink
        UNICODE_STRING ustrLinkName = pDeviceExtObject->ustrDeviceName;
        IoDeleteSymbolicLink(&ustrLinkName);
        //Delete device
        pNextDeviceObject = pNextDeviceObject->NextDevice;
        IoDeleteDevice(pDeviceExtObject->pFdo);
    }

    KdPrint(("HelloDDKUnload End.\n"));
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDispatch(IN PDEVICE_OBJECT pDeviceObject,IN  PIRP pIrp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);

    KdPrint(("HelloDDK:Enter Dispatch.\n"));
    NTSTATUS status = STATUS_SUCCESS;

    //Deal pIrp
    pIrp->IoStatus.Status = status;
    pIrp->IoStatus.Information = 0;

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    
    KdPrint(("HelloDDK:Dispatch End.\n"));
    return status;
}

#pragma INITCODE
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject)
{
    KdPrint(("HelloDDK:Enter CreateDevice.\n"));
    NTSTATUS status;
    PDEVICE_OBJECT pDeviceObject;
    PDEVICE_EXTENSION pDeviceExtObject;

    pDeviceExtObject = (PDEVICE_EXTENSION)ExAllocatePoolWithTag(NonPagedPool, sizeof(DEVICE_EXTENSION), 0);

    //Create Device
    UNICODE_STRING ustrDeviceName;
    RtlInitUnicodeString(&ustrDeviceName, L"\\Device\\HelloDDKDevice");
    status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &ustrDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDeviceObject);
    if (!NT_SUCCESS(status))
        return status;

    pDeviceObject->Flags |= DO_BUFFERED_IO;
    pDeviceExtObject->pFdo = pDeviceObject;
    pDeviceExtObject->ustrDeviceName = ustrDeviceName;

    //Create SymLink
    UNICODE_STRING ustrSymLinkName;
    RtlInitUnicodeString(&ustrSymLinkName, L"\\??\\HelloDDKSymLinkName");
    pDeviceExtObject->ustrSymLinkName = ustrSymLinkName;

    status = IoCreateSymbolicLink(&ustrSymLinkName, &ustrDeviceName);
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(pDeviceObject);
        return status;
    }

    KdPrint(("HelloDDK:CreateDevice End.\n"));
    return status;
}
