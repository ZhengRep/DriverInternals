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

//���Pnp���豸���			Pnp Manager ����������������		Pnp Manager�������ĵײ��豸����
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject, IN PDEVICE_OBJECT pPhysicalDeviceObject);
//����PnpIrp�Ļص�����
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp);
NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp);
NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pDevExt, IN PIRP pIrp);
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT pFdo, IN PIRP pIrp);
