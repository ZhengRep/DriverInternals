#pragma once

#include<wdm.h>

#define PAGEDCODE		CODE_SEG("page")
#define LOCKEDCODE		CODE_SEG()
#define INITCODE		CODE_SEG("init")

#define PAGEDDATA		CODE_SEG("page")
#define LOCKEDDATA		CODE_SEG()
#define INITDATA		CODE_SEG("init")

//Get Array Size
#define arraysize(p) (sizeof(p) / sizeof(p[0]))

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pFdo;
	PDEVICE_OBJECT pNextStachDevice;
	UNICODE_STRING ustrDeviceName;
	UNICODE_STRING ustrSymLinkName;
}DEVICE_EXTENSION, * PDEVICE_EXTENSION;

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN UNICODE_STRING pRegisterPath);
VOID HelloWDMUnload(IN PDRIVER_OBJECT pDriverObject);

//完成Pnp的设备添加			Pnp Manager 传下来的驱动对象		Pnp Manager传下来的底层设备对象
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject, IN PDEVICE_OBJECT pPhysicalDeviceObject);
//处理PnpIrp的回调函数
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp);
NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp);
NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp);
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp);
