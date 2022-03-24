#pragma once

#ifdef _cpluscplus
extern "C" {
#endif
#include<ntddk.h>
#ifdef _cpluscplus
}
#endif

#define PAGEDCODE		CODE_SEG("page")
#define LOCKEDCODE		CODE_SEG()
#define INITCODE		CODE_SEG("init")

#define PAGEDDATA		CODE_SEG("page")
#define LOCKEDDATA		CODE_SEG()
#define INITDATA		CODE_SEG("init")

//Get Array Size
#define arraysize(p) {sizeof(p) / sizeof(p[0])}

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING ustrDeviceName;
	UNICODE_STRING ustrSymLinkName;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject);
NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT pDeviceObject, IN  PIRP pIrp);
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);
VOID CreateFileTest();
VOID OperateFileAttributes();