#include "FileTest.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN UNICODE_STRING pRegisterPath)
{
	UNREFERENCED_PARAMETER(pRegisterPath);

	KdPrint(("Enter DriverEntry.\n"));
	NTSTATUS status;

	//Dispatch
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchRoutine;
	//Unload
	pDriverObject->DriverUnload = DriverUnload;

	//Create device\n
	status = CreateDevice(pDriverObject);

	//File Test
	CreateFileTest();
	OperateFileAttributes();
	
	KdPrint(("DriverEntry End.\n"));
	return status;
}

#pragma INITCODE
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Enter CreateDevice.\n"));
	NTSTATUS status;
	PDEVICE_OBJECT pDeviceObject;
	PDEVICE_EXTENSION pDeviceExtObject;
	pDeviceExtObject = (PDEVICE_EXTENSION)ExAllocatePool(NonPagedPool, sizeof(DEVICE_EXTENSION));

	//Create Device
	UNICODE_STRING ustrDeviceName;
	RtlInitUnicodeString(&ustrDeviceName, L"\\Device\\MyDDKDevice");
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &ustrDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDeviceObject);
	if (!NT_SUCCESS(status))
		return status;

	pDeviceObject->Flags |= DO_BUFFERED_IO;
	pDeviceExtObject->pDeviceObject = pDeviceObject;
	pDeviceExtObject->ustrDeviceName = ustrDeviceName;

	//Create SymLink
	UNICODE_STRING ustrSymLinkName;
	RtlInitUnicodeString(&ustrSymLinkName, L"\\??\\MyDDKSymLinkName");
	pDeviceExtObject->ustrSymLinkName = ustrSymLinkName;

	status = IoCreateSymbolicLink(&ustrSymLinkName, &ustrDeviceName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteSymbolicLink(&ustrSymLinkName);
		status = IoCreateSymbolicLink(&ustrSymLinkName, &ustrDeviceName);
		if (!NT_SUCCESS(status))
		{
			IoDeleteDevice(pDeviceObject);
			return status;
		}
	}

	KdPrint(("CreateDevice End.\n"));
	return status;
}

#pragma PAGEDCODE
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Enter DriverUnload\n"));
	PDEVICE_OBJECT pNextDeviceObject = pDriverObject->DeviceObject;

	while (pNextDeviceObject != NULL)
	{
		PDEVICE_EXTENSION pDeviceExtObject = (PDEVICE_EXTENSION)pNextDeviceObject->DeviceExtension;
		//Delete SymLink
		UNICODE_STRING ustrLinkName = pDeviceExtObject->ustrDeviceName;
		IoDeleteSymbolicLink(&ustrLinkName);
		//Delete device
		pNextDeviceObject = pNextDeviceObject->NextDevice;
		IoDeleteDevice(pDeviceExtObject->pDeviceObject);
	}

	KdPrint(("DriverUnload End.\n"));
}

#pragma PAGEDCODE
NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT pDeviceObject, IN  PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	KdPrint(("Enter DispatchRountine.\n"));
	NTSTATUS status = STATUS_SUCCESS;

	//Deal pIrp
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("DispatchRoutine End.\n"));
	return status;
}

#pragma INITCODE
VOID CreateFileTest()
{
	HANDLE hFile;
	OBJECT_ATTRIBUTES objectAtrributes;
	IO_STATUS_BLOCK ioStatus;
	UNICODE_STRING unstLogFileName;

	RtlInitUnicodeString(&unstLogFileName, L"\\??\\C:\\LogTest.txt");// "\\Device\\HarddiskVolume1\\LogFile.txt"
	InitializeObjectAttributes(&objectAtrributes, &unstLogFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = ZwCreateFile(&hFile, GENERIC_WRITE, &objectAtrributes, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if (NT_SUCCESS(status))
	{
		KdPrint(("Create File Successfully.\n"));
	}
	else
	{
		KdPrint(("Create File Unsuccessfully.\n"));
	}


	ZwClose(hFile);
}

#pragma INITCODE
VOID OperateFileAttributes()
{
	HANDLE hFile;
	OBJECT_ATTRIBUTES objectAtrributes;
	IO_STATUS_BLOCK ioStatus;
	UNICODE_STRING unstLogFileName;

	RtlInitUnicodeString(&unstLogFileName, L"\\??\\C:\\LogTest.txt");// "\\Device\\HarddiskVolume1\\LogFile.txt"
	InitializeObjectAttributes(&objectAtrributes, &unstLogFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = ZwOpenFile(&hFile, GENERIC_ALL, &objectAtrributes, &ioStatus, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Open file is wrong.\n"));
	}

	FILE_STANDARD_INFORMATION fsi;
	status = ZwQueryInformationFile(hFile, &ioStatus, &fsi, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (NT_SUCCESS(status))
	{
		KdPrint(("The file length is %u.\n", fsi.EndOfFile.QuadPart));
	}
	else
	{
		KdPrint(("Get fileinfo is wrong.\n"));
	}

	FILE_POSITION_INFORMATION fpi;
	fpi.CurrentByteOffset.QuadPart = 100161;
	status = ZwSetInformationFile(hFile, &ioStatus, &fpi, sizeof(FILE_POSITION_INFORMATION), FilePositionInformation);
	if (NT_SUCCESS(status))
	{
		KdPrint(("Update file length successfully.\n"));
	}

	ZwClose(hFile);
}

#pragma INITCODE
VOID ListEntryTest()
{
	LIST_ENTRY listEntryHead;
	InitializeListHead(&listEntryHead);

	PMYDATASTRUCT pMyData;
	//Insert
	KdPrint(("Start to Insert Entry."));
	for (int i = 0; i < 10; i++)
	{
		pMyData = (PMYDATASTRUCT)ExAllocatePool(NonPagedPool, sizeof(MYDATASTRUCT));
		pMyData->id = i;
		InsertHeadList(&listEntryHead, &(pMyData->ListEntry));
	}

	//Remove
	KdPrint(("Remove and show Entry id."));
	while (!IsListEmpty(&listEntryHead))
	{
		PLIST_ENTRY pEntry = RemoveTailList(&listEntryHead);
		PMYDATASTRUCT pMyData = CONTAINING_RECORD(pEntry, MYDATASTRUCT, ListEntry);
		KdPrint(("MyData id is %u.\n", pMyData->id));
		ExFreePool(pMyData);
	}
}

#pragma INITCODE
VOID LookasideTest()
{
	PAGED_LOOKASIDE_LIST pageLookasideList;
	ExInitializeLookasideListEx(&pageLookasideList, NULL, NULL, NonPagedPool, 0, 1024, 0, 0);

#define ARRAYNUMBER 15
	PMYDATASTRUCT MyBufferArray[ARRAYNUMBER];

	//Simulation of allcate memory frenquntly
	for (int i = 0; i < 15; i++)
	{
		MyBufferArray[i] = (PMYBUFFERBLOCK)ExAllocateFromPagedLookasideList(&pageLookasideList);
	}

	//Free momery
	for (int i = 0; i < 15; i++)
	{
		ExFreeToPagedLookasideList(&pageLookasideList, MyBufferArray[i]);
		MyBufferArray[i] = NULL;
	}

	ExDeletePagedLookasideList(&pageLookasideList);
}