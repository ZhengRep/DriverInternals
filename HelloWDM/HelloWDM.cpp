#include "HelloWDM.h"
/*
Common Code:
KdPrint(("Enter \n"));

	KdPrint(("Enter \n"));
	NTSTATUS status = STATUS_SUCCESS;




	KdPrint(("Leave \n"));
	return status;

Todo:

*/

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN UNICODE_STRING pRegisterPath)
{
	UNREFERENCED_PARAMETER(pRegisterPath);

	KdPrint(("HelloWDM:Enter DriverEntry.\n"));
	DbgPrint("HelloWDM:Enter DriverEntry.\n");
	NTSTATUS status = STATUS_SUCCESS;

	//add device
	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;

	//pnp
	pDriverObject->MajorFunction[IRP_MJ_PNP] = HelloWDMPnp;

	//Dispatch
	//pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ;
	//pDriverObject->MajorFunction[IRP_MJ_CREATE] = ;
	//pDriverObject->MajorFunction[IRP_MJ_READ] = ;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloWDMDispatchRoutine;
	
	//Unload
	pDriverObject->DriverUnload = HelloWDMUnload;

	KdPrint(("HelloWDM:DriverEntry End.\n"));

	return status;
}

VOID HelloWDMUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	PAGED_CODE(); //When code run above APC_LEVER will be strop
	KdPrint(("Enter HelloWDMUnload\n"));
	KdPrint(("Leave HelloWDMUnload\n"));
}

NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pFdo);

	KdPrint(("Enter HelloWDMDispatch\n"));

	NTSTATUS status = STATUS_SUCCESS;
	
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloWDMDispatch\n"));
	return status;
}

NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject, IN PDEVICE_OBJECT pPhysicalDeviceObject)
{
	KdPrint(("Enter HelloWDMDAddDevice\n"));
	NTSTATUS status = STATUS_SUCCESS;

	//Create FDO
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\HelloWDMDevice");

	PDEVICE_OBJECT pFdo;
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pFdo);
	if (!NT_SUCCESS(status))
		return status;

	PDEVICE_EXTENSION pDevExtObj = (PDEVICE_EXTENSION)pFdo->DeviceExtension;
	pDevExtObj->pFdo = pFdo;
	pDevExtObj->ustrDeviceName = devName;
	pDevExtObj->pNextStachDevice = IoAttachDeviceToDeviceStack(pFdo, pPhysicalDeviceObject);

	//Create SymbolLink
	UNICODE_STRING symbolLinkName;
	RtlInitUnicodeString(&symbolLinkName, L"\\DosDevices\\HelloWDMSymbolLinkName");
	pDevExtObj->ustrSymLinkName = symbolLinkName;

	status = IoCreateSymbolicLink(&symbolLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteSymbolicLink(&symbolLinkName);
		status = IoCreateSymbolicLink(&symbolLinkName, &devName);
		if (!NT_SUCCESS(status))
			return status;
		
	}

	//Flags
	pFdo->Flags |= DO_BUFFERED_IO | DO_DIRECT_IO;
	pFdo->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Leave HelloWDMDAddDevice\n"));
	return status;
}

NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp)
{
	KdPrint(("Enter HelloWDMDPnp\n"));
	NTSTATUS status = STATUS_SUCCESS;

	//Deal with IRP_MJ_REMOVE_DEVICE
	static NTSTATUS (*FuncTable[])(PDEVICE_EXTENSION pDevExt, PIRP pIrp) = {
		DefaultPnpHandler,		// ** IRP_MN_START_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_REMOVE_DEVICE
		HandleRemoveDevice,		// ** IRP_MN_REMOVE_DEVICE
		DefaultPnpHandler,		// IRP_MN_CANCEL_REMOVE_DEVICE
		DefaultPnpHandler,		// IRP_MN_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_CANCEL_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_DEVICE_RELATIONS
		DefaultPnpHandler,		// IRP_MN_QUERY_INTERFACE
		DefaultPnpHandler,		// IRP_MN_QUERY_CAPABILITIES
		DefaultPnpHandler,		// IRP_MN_QUERY_RESOURCES
		DefaultPnpHandler,		// IRP_MN_QUERY_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,		// IRP_MN_QUERY_DEVICE_TEXT
		DefaultPnpHandler,		// IRP_MN_FILTER_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,		// 
		DefaultPnpHandler,		// IRP_MN_READ_CONFIG
		DefaultPnpHandler,		// IRP_MN_WRITE_CONFIG
		DefaultPnpHandler,		// IRP_MN_EJECT
		DefaultPnpHandler,		// IRP_MN_SET_LOCK
		DefaultPnpHandler,		// IRP_MN_QUERY_ID
		DefaultPnpHandler,		// IRP_MN_QUERY_PNP_DEVICE_STATE
		DefaultPnpHandler,		// IRP_MN_QUERY_BUS_INFORMATION
		DefaultPnpHandler,		// IRP_MN_DEVICE_USAGE_NOTIFICATION
		DefaultPnpHandler,		// IRP_MN_SURPRISE_REMOVAL
	};

#ifdef DEBUG
	static char* FuncNameTable[] = {
		"IRP_MN_START_DEVICE",
		"IRP_MN_QUERY_REMOVE_DEVICE",
		"IRP_MN_REMOVE_DEVICE",
		"IRP_MN_CANCEL_REMOVE_DEVICE",
		"IRP_MN_STOP_DEVICE",
		"IRP_MN_QUERY_STOP_DEVICE",
		"IRP_MN_CANCEL_STOP_DEVICE",
		"IRP_MN_QUERY_DEVICE_RELATIONS",
		"IRP_MN_QUERY_INTERFACE",
		"IRP_MN_QUERY_CAPABILITIES",
		"IRP_MN_QUERY_RESOURCES",
		"IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
		"IRP_MN_QUERY_DEVICE_TEXT",
		"IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
		"",
		"IRP_MN_READ_CONFIG",
		"IRP_MN_WRITE_CONFIG",
		"IRP_MN_EJECT",
		"IRP_MN_SET_LOCK",
		"IRP_MN_QUERY_ID",
		"IRP_MN_QUERY_PNP_DEVICE_STATE",
		"IRP_MN_QUERY_BUS_INFORMATION",
		"IRP_MN_DEVICE_USAGE_NOTIFICATION",
		"IRP_MN_SURPRISE_REMOVAL",
};
#endif // DEBUG

	//Deal With Another IRP
	//Get Stack Location
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pFdo->DeviceExtension;
	
	ULONG uFuncNameIndex = stack->MinorFunction;
	if (uFuncNameIndex >= arraysize(FuncTable))
	{
		status = DefaultPnpHandler(pDevExt, pIrp);
		return status;
	}

#ifdef DEBUG
	KdPrint(("PNP Request %s\n", FuncNameTable[uFuncNameIndex]));
#endif 
	status = (FuncTable[uFuncNameIndex](pDevExt, pIrp));

	KdPrint(("Leave HelloWDMDPnp\n"));
	return status;
}

NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp)
{
	KdPrint(("Enter DefualtPnpHandler\n"));

	NTSTATUS status = STATUS_SUCCESS;

	//Jump current stack and deliver to next device
	IoSkipCurrentIrpStackLocation(pIrp);
	KdPrint(("Leave DefualtPnpHandler\n"));

	status = IoCallDriver(pDevExt->pNextStachDevice, pIrp);

	return status;
}

NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp)
{
	KdPrint(("Enter HandleRemoveDevice\n"));
	NTSTATUS status = STATUS_SUCCESS;

	//Return status
	pIrp->IoStatus.Status = status;
	status = DefaultPnpHandler(pDevExt, pIrp);
	//Delete SymbolLinkName
	IoDeleteSymbolicLink(&(pDevExt->ustrDeviceName));
	//Detarch device
	if(pDevExt->pNextStachDevice)
		IoDetachDevice(pDevExt->pNextStachDevice);

	//Delete device
	IoDetachDevice(pDevExt->pFdo);

	KdPrint(("Leave HandleRemoveDevice\n"));
	return status;
}
