/***************************************************************** 
�ļ��� : WssLockKey.c 
���� : ���̹������� 
���� : sinister 
����޸����� : 2007-02-26 
*****************************************************************/ 
#include "WssLockKey.h" 
//#include "kbdmou.h"
//#include <ntddmou.h>
#define IOCTL_INTERNAL_MOUSE_CONNECT    CTL_CODE(FILE_DEVICE_MOUSE, 0x0080, METHOD_NEITHER, FILE_ANY_ACCESS)

BOOLEAN g_bUsbKeyBoard = FALSE, g_bUsbMouse = FALSE;

USHORT g_UnitId;
PDEVICE_OBJECT g_MouseDev;

PSERVICE_CALLBACK_ROUTINE g_SeviceCallback;

typedef NTSTATUS
(NTAPI*	INTERNIOCTL)(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
	);
INTERNIOCTL g_OldInternIoCtl;
NTSTATUS
	MouFilter_InternIoCtl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
	)
{
	PIO_STACK_LOCATION          irpStack;
	PDEVICE_EXTENSION           devExt;
	PCONNECT_DATA               connectData;
	NTSTATUS                    status = STATUS_SUCCESS;

	devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	Irp->IoStatus.Information = 0;
	irpStack = IoGetCurrentIrpStackLocation(Irp);

	switch (irpStack->Parameters.DeviceIoControl.IoControlCode) 
	{
		//
		// Connect a mouse class device driver to the port driver.
		//
	case IOCTL_INTERNAL_MOUSE_CONNECT:
		//
		// Only allow one connection.
		//
		if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(CONNECT_DATA)) {
				//
				// invalid buffer
				//
				status = STATUS_INVALID_PARAMETER;
				break;
		}

		//
		// Copy the connection parameters to the device extension.
		//
		connectData = ((PCONNECT_DATA)
			(irpStack->Parameters.DeviceIoControl.Type3InputBuffer));

		g_MouseDev = connectData->ClassDeviceObject;
		g_SeviceCallback = *(PSERVICE_CALLBACK_ROUTINE)connectData->ClassService;
		DbgPrint("KeyClass = %x,Service Callback = %x\n", g_MouseDev, g_SeviceCallback);
		break;
	}
	return g_OldInternIoCtl(DeviceObject, Irp);
}
NTSTATUS KeyDriverEntry( IN PDRIVER_OBJECT KeyDriverObject, IN PUNICODE_STRING RegistryPath )
{ 
	UNICODE_STRING KeyDeviceName; 
	PDRIVER_OBJECT KeyDriver; 
	PDEVICE_OBJECT UsbKeyBoardDeviceObject, UsbMouseDeviceObject, HookDeviceObject; 
	NTSTATUS ntStatus; 
	ULONG i; 
	// 
	// �����豸��������ʹ�� 
	// 
	WCHAR szDeviceName[MAXLEN + MAXLEN] = {0}; 
	//KeyDriverObject->DriverUnload = KeyDriverUnload; 
	// 
	// �ȳ��Ի�� USB �����豸��������ɹ���ҽ� USB ���� 
	// 
	// ע�⣺��Ϊ USB �����豸�����̶����Ҽ�ʹ�õ�����Ҳ�޷� 
	// ʹ�� IoGetDeviceObjectPointer() ���������豸���Ƶõ��� 
	// �豸����������������ֻ���Լ�ö�� USB �豸ջ�����õ� 
	// USB �����豸�����йҽ� 
	// 
	ntStatus = GetUsbKeybordDevice( &UsbKeyBoardDeviceObject, &UsbMouseDeviceObject); 
	if ( NT_SUCCESS( ntStatus )) 
	{ 
		if(UsbKeyBoardDeviceObject != NULL)
		{
			// 
			// ����ʹ�ã�USB �����豸 kbdhid û���豸��ֻ�������� 
			// ���������ӡΪ�� 
			// 
			RtlInitUnicodeString( &KeyDeviceName, szDeviceName ); // USB KEYBOARD 
			DbgPrint( "KeyDeviceName:%S\n", KeyDeviceName.Buffer ); 
			DbgPrint( "Find UsbKeybordDriverName:%S\n", UsbKeyBoardDeviceObject->DriverObject->DriverName.Buffer); 
			
			//
			// 
			// �ҽ� USB �����豸 
			// 
			//ntStatus = AttachUSBKeyboardDevice( UsbKeyBoardDeviceObject, KeyDriverObject ); 
			if ( !NT_SUCCESS( ntStatus ) ) 
			{ 
				DbgPrint( "Attach USB Keyboard Device to failed!\n" ); 
				return STATUS_INSUFFICIENT_RESOURCES; 
			} 
		}
		else if(UsbMouseDeviceObject != NULL)
		{
			DbgPrint( "Find UsbMouse DriverName:%S\n", UsbMouseDeviceObject->DriverObject->DriverName.Buffer); 
			HookDeviceObject = UsbMouseDeviceObject->AttachedDevice;
			g_OldInternIoCtl = (INTERNIOCTL)(HookDeviceObject->DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL]);
			DbgPrint( "Find UsbMouse Hook DriverName:%S, Driver Function:%X\n", HookDeviceObject->DriverObject->DriverName.Buffer, g_OldInternIoCtl); 
			HookDeviceObject->DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = MouFilter_InternIoCtl;
			// 
			// �ҽ� USB ����豸
			// 
			//ntStatus = AttachUSBKeyboardDevice( UsbMouseDeviceObject, KeyDriverObject ); 
			if ( !NT_SUCCESS( ntStatus ) ) 
			{ 
				DbgPrint( "Attach USB Keyboard Device to failed!\n" ); 
				return STATUS_INSUFFICIENT_RESOURCES; 
			} 
		}

	} 
	return STATUS_SUCCESS; 
	if (UsbKeyBoardDeviceObject == NULL || UsbMouseDeviceObject == NULL)
	{ 
		// 
		// ���û�� USB ���̣����Թҽ� PS/2 �����豸 
		// 
		RtlInitUnicodeString( &KeyDeviceName, PS2KEYBOARDNAME ); 
		ntStatus = AttachPS2KeyboardDevice( &KeyDeviceName, 
			KeyDriverObject, 
			&KeyDriver ); 
		if ( !NT_SUCCESS( ntStatus ) || KeyDriver == NULL ) 
		{ 
			DbgPrint( "Attach PS2 Keyboard Device to failed!\n" ); 
			return STATUS_INSUFFICIENT_RESOURCES; 
		} 
	} 
	// 
	// ����û�й����������̣��������˰�������������������ֹ 
	// ���ߡ���������ʱ���������ߣ�����Ҳ�����봦���������� 
	// 
	//KeyDriverObject->MajorFunction[IRP_MJ_READ] = KeyReadPassThrough; 
	return STATUS_SUCCESS; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : ϵͳ���� 
// ����ģ�� : ���̹���ģ�� 
//////////////////////////////////////////////////////////////// 
// ���� : ����ȡ����������첽 IRP�����ʧ����ȴ��û������� 
// ж�ؼ��̹������� 
// ע�� : ȡ�� IRP ������ 2000 ϵͳ�Ͽ��Գɹ����� XP / 2003 �� 
// ����Ҫ�ȴ��û��������Ժ��д����� 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2005.12.27 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
//////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
VOID KeyDriverUnload( PDRIVER_OBJECT KeyDriver ) 
{ 
	PDEVICE_OBJECT KeyFilterDevice ; 
	PDEVICE_OBJECT KeyDevice ; 
	PDEVICE_EXTENSION KeyExtension; 
	PIRP Irp; 
	NTSTATUS ntStatus; 
	KeyFilterDevice = KeyDriver->DeviceObject; 
	KeyExtension = ( PDEVICE_EXTENSION ) KeyFilterDevice->DeviceExtension; 
	KeyDevice = KeyExtension->TargetDevice; 
	IoDetachDevice( KeyDevice ); 
	// 
	// ������� IRP δ��ɣ��ҵ�ǰ IRP ��Ч����ȡ����� IRP 
	// 
	if ( KeyExtension->IrpsInProgress >0 &&KeyDevice->CurrentIrp != NULL ) 
	{ 
		if ( CancelKeyboardIrp( KeyDevice->CurrentIrp ) ) 
		{ 
			// 
			// �ɹ���ֱ���˳�ɾ�����̹����豸 
			// 
			DbgPrint( "CancelKeyboardIrp() is ok\n" ); 
			goto __End; 
		} 
	} 
	// 
	// ���ȡ��ʧ�ܣ���һֱ�ȴ����� 
	// 
	while ( KeyExtension->IrpsInProgress >0 ) 
	{ 
		DbgPrint( "Irp Count:%d\n", KeyExtension->IrpsInProgress ); 
	} 
__End: 
	IoDeleteDevice( KeyFilterDevice ); 
	return ; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : ���̹���ģ�� 
///////////////////////////////////////////////////////////////// 
// ���� : ȡ�� IRP ���� 
// ע�� : �����������Ϊ����� UNLOAD ����ʹ�ã����������в��� 
// ʹ�ô˷�����ȡ�� IRP 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2007.02.20 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
///////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
BOOLEAN CancelKeyboardIrp( IN PIRP Irp ) 
{ 
	if ( Irp == NULL ) 
	{ 
		DbgPrint( "CancelKeyboardIrp: Irp error\n" ); 
		return FALSE; 
	} 
	// 
	// ������Щ�ж�Ӧ�ò��Ǳ���ģ������ CancelRoutine �ֶΣ� 
	// ��Ϊ IoCancelIrp() ���������ж��ˡ���ֻ��ƫִ��������� :)�� 
	// С��˵ ���ǡ�ƫִ��˼��ƶ����������̵ġ�����һ�д����ռ 
	// ������ :D����֪ xiaonvwu ����������θ��룿:DDD 
	// 
	// 
	// �������ȡ����û��ȡ��������ֱ�ӷ��� FALSE 
	// 
	if ( Irp->Cancel || Irp->CancelRoutine == NULL ) 
	{ 
		DbgPrint( "Can't Cancel the irp\n" ); 
		return FALSE; 
	} 
	if ( FALSE == IoCancelIrp( Irp ) ) 
	{ 
		DbgPrint( "IoCancelIrp() to failed\n" ); 
		return FALSE; 
	} 
	// 
	// ȡ�������������Ϊ�� 
	// 
	IoSetCancelRoutine( Irp, NULL ); 
	return TRUE; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : �豸ջ��Ϣģ�� 
///////////////////////////////////////////////////////////////// 
// ���� : ���� DEVICE_OBJECT �� AttachedDevice ���ҵ� USB ���� 
// �豸����Ϊ kbdhid �Ĺ���������Upper Filter Driver�� 
// ע�� : 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2005.06.02 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
///////////////////////////////////////////////////////////////// 
// �޸��� : sinister 
// �޸����� : 2007.2.12 
// �޸����� : Ϊƥ�� USB ��������������Ӧ���޸� 
///////////////////////////////////////////////////////////////// 
ULONG GetAttachedDeviceInfo( IN PDEVICE_OBJECT DevObj ) 
{ 
	PDEVICE_OBJECT DeviceObject; 
	ULONG uFound = FALSE; 
	if ( DevObj == NULL ) 
	{ 
		DbgPrint( "DevObj is NULL!\n" ); 
		return FALSE; 
	} 
	DeviceObject = DevObj->AttachedDevice; 
	while ( DeviceObject ) 
	{ 
		// 
		// һЩ OBJECT �����ƶ����ڷ�ҳ������Ȼ�󲿷�ʱ�򲻻ᱻ������ȥ���� 
		// ��һ���㹻�ˡ������Ǿ���̸֮ 
		// 
		if ( MmIsAddressValid( DeviceObject->DriverObject->DriverName.Buffer ) ) 
		{ 
			DbgPrint( "Attached Driver Name:%S,Attached Driver Address:0x%x,Attached DeviceAddress:0x%x\n", 
				DeviceObject->DriverObject->DriverName.Buffer, 
				DeviceObject->DriverObject, 
				DeviceObject ); 
			// 
			// �ҵ� USB ���������� kbdhid �豸��ô���ҵ��˾Ͳ������� 
			// 
			if ( _wcsnicmp( DeviceObject->DriverObject->DriverName.Buffer, 
				KDBDEVICENAME, 
				wcslen( KDBDEVICENAME ) ) == 0 ) 
			{ 
				DbgPrint( "Found kbdhid Device\n" ); 
				uFound = 1;
				break; 
			} 
			// 
			// �ҵ� USB��������� mouhid �豸��ô���ҵ��˾Ͳ������� 
			// 
			if ( _wcsnicmp( DeviceObject->DriverObject->DriverName.Buffer, 
				MOUDEVICENAME, 
				wcslen( MOUDEVICENAME ) ) == 0 ) 
			{ 
				DbgPrint( "Found mouhid Device\n" ); 
				uFound = 2;
				break; 
			} 
		} 
		DeviceObject = DeviceObject->AttachedDevice; 
	} 
	return uFound; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : �豸ջ��Ϣģ�� 
///////////////////////////////////////////////////////////////// 
// ���� : �� DEVICE_OBJECT �еõ��豸���������Ʋ���ӡ��ַ 
// ע�� : ��������ֻ�Ǵ�ӡ��Ϣ����ͬ����ʹ����Ӧ�û����޸� 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2006.05.02 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
///////////////////////////////////////////////////////////////// 
// �޸��� : sinister 
// �޸����� : 2007.2.12 
// �޸����� : ��ӡ�� USB �����������豸���ƣ���������ʹ�� 
///////////////////////////////////////////////////////////////// 
VOID GetDeviceObjectInfo( IN PDEVICE_OBJECT DevObj ) 
{ 
	POBJECT_HEADER ObjectHeader; 
	POBJECT_HEADER_NAME_INFO ObjectNameInfo; 
	if ( DevObj == NULL ) 
	{ 
		DbgPrint( "DevObj is NULL!\n" ); 
		return; 
	} 
	// 
	// �õ�����ͷ 
	// 
	ObjectHeader = OBJECT_TO_OBJECT_HEADER( DevObj ); 
	if ( ObjectHeader ) 
	{ 
		// 
		// ��ѯ�豸���Ʋ���ӡ 
		// 
		ObjectNameInfo = OBJECT_HEADER_TO_NAME_INFO( ObjectHeader ); 
		if ( ObjectNameInfo &&ObjectNameInfo->Name.Buffer ) 
		{ 
			DbgPrint( "Device Name:%S - Device Address:0x%x\n", 
				ObjectNameInfo->Name.Buffer, 
				DevObj ); 
			// 
			// ���� USB �����豸����һ��ȫ�� BUFFER �Ϊ����ʱ��ʾ 
			// �ã�û��ʵ�ʵĹ�����; 
			// 
			RtlZeroMemory( szUsbDeviceName, sizeof( szUsbDeviceName ) ); 
			wcsncpy( szUsbDeviceName, 
				ObjectNameInfo->Name.Buffer, 
				ObjectNameInfo->Name.Length / sizeof( WCHAR ) ); 
		} 
		// 
		// ����û�����Ƶ��豸�����ӡ NULL 
		// 
		else if ( DevObj->DriverObject ) 
		{ 
			DbgPrint( "Driver Name:%S - Device Name:%S - Driver Address:0x%x - Device Address:0x%x\n", 
				DevObj->DriverObject->DriverName.Buffer, 
				L"NULL", 
				DevObj->DriverObject, 
				DevObj ); 
		} 
	} 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : ���̹���ģ�� 
///////////////////////////////////////////////////////////////// 
// ���� : �õ� USB ���� hidusb ���������󣬲��������������豸 
// ���󣬹��˳� USB �����豸�������豸���󷵻� 
// ע�� : 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2007.02.13 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
///////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
NTSTATUS GetUsbKeybordDevice( OUT PDEVICE_OBJECT* UsbKeyBoardDeviceObject, OUT PDEVICE_OBJECT* UsbMouseDeviceObject) 
{ 
	UNICODE_STRING DriverName; 
	PDRIVER_OBJECT DriverObject = NULL; 
	PDEVICE_OBJECT DeviceObject = NULL; 
	BOOLEAN bFound = FALSE; 
	ULONG	uGetInfo = 0;
	RtlInitUnicodeString( &DriverName, USBKEYBOARDNAME ); 
	ObReferenceObjectByName( &DriverName, 
		OBJ_CASE_INSENSITIVE, 
		NULL, 
		0, 
		( POBJECT_TYPE ) IoDriverObjectType, 
		KernelMode, 
		NULL, 
		&DriverObject ); 
	if ( DriverObject == NULL ) 
	{ 
		DbgPrint( "Not found USB Keyboard Device hidusb!\n" ); 
		return STATUS_UNSUCCESSFUL; 
	} 
	*UsbKeyBoardDeviceObject = NULL;
	*UsbMouseDeviceObject = NULL;
	DeviceObject = DriverObject->DeviceObject; 
	while ( DeviceObject ) 
	{ 
		GetDeviceObjectInfo( DeviceObject ); 
		if ( DeviceObject->AttachedDevice ) 
		{ 
			// 
			// ���� USB �����豸 
			// 
			uGetInfo = GetAttachedDeviceInfo( DeviceObject );
			if (uGetInfo == 1 && !g_bUsbKeyBoard)	//�ҵ�USB�����豸
			{ 
				// 
				// �ҵ��򷵻� USB �����豸���� 
				// 
				g_bUsbKeyBoard = TRUE;
				*UsbKeyBoardDeviceObject = DeviceObject;
				if (bFound)
					goto __End;
				else
					bFound = TRUE; 				 
			}
			else if(uGetInfo == 2 && !g_bUsbMouse) //�ҵ�USB����豸
			{
				g_bUsbMouse = TRUE;
				*UsbMouseDeviceObject = DeviceObject;
				if (bFound)
					goto __End;
				else
					bFound = TRUE;
			}
		} 
		DeviceObject = DeviceObject->NextDevice; 
	} 
__End: 
	return STATUS_SUCCESS; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : ���̹���ģ�� 
//////////////////////////////////////////////////////////////// 
// ���� : ���������豸���丽�ӵ���Ҫ���ٵ��豸�ϣ������豸��� 
// ��Ϣ�����ظ��Ӻ���������� 
// ע�� : �˺������ҽ� USB �����豸 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2005.12.27 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
//////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
NTSTATUS AttachUSBKeyboardDevice( IN PDEVICE_OBJECT UsbDeviceObject, IN PDRIVER_OBJECT DriverObject ) 
{ 
	PDEVICE_OBJECT DeviceObject; 
	PDEVICE_OBJECT TargetDevice; 
	PDEVICE_EXTENSION DevExt; 
	NTSTATUS ntStatus; 
	// 
	// ���������豸���� 
	// 
	ntStatus = IoCreateDevice( DriverObject, 
		sizeof( DEVICE_EXTENSION ), 
		NULL, 
		FILE_DEVICE_UNKNOWN, 
		0, 
		FALSE, 
		&DeviceObject ); 
	if ( !NT_SUCCESS( ntStatus ) ) 
	{ 
		DbgPrint( "IoCreateDevice() 0x%x!\n", ntStatus ); 
		return ntStatus; 
	} 
	DevExt = ( PDEVICE_EXTENSION ) DeviceObject->DeviceExtension; 
	// 
	// ��ʼ�������� 
	// 
	KeInitializeSpinLock( &DevExt->SpinLock ); 
	// 
	// ��ʼ�� IRP ������ 
	// 
	DevExt->IrpsInProgress = 0; 
	// 
	// �������豸���󸽼���Ŀ���豸����֮�ϣ������ظ��Ӻ��ԭ�豸���� 
	// 
	TargetDevice = IoAttachDeviceToDeviceStack( DeviceObject, UsbDeviceObject ); 
	if ( !TargetDevice ) 
	{ 
		IoDeleteDevice( DeviceObject ); 
		DbgPrint( "IoAttachDeviceToDeviceStack() 0x%x!\n", ntStatus ); 
		return STATUS_INSUFFICIENT_RESOURCES; 
	} 
	// 
	// ��������豸��Ϣ 
	// 
	DevExt->DeviceObject = DeviceObject; 
	DevExt->TargetDevice = TargetDevice; 
	// 
	// ���ù����豸�����Ϣ���־ 
	// 
	DeviceObject->Flags |= ( DO_BUFFERED_IO | DO_POWER_PAGABLE ); 
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING; 
	return STATUS_SUCCESS; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : ���̹���ģ�� 
//////////////////////////////////////////////////////////////// 
// ���� : ���������豸���丽�ӵ���Ҫ���ٵ��豸�ϣ������豸��� 
// ��Ϣ�����ظ��Ӻ���������� 
// ע�� : �˺������ҽ� PS/2 �����豸 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2005.12.27 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
//////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
NTSTATUS AttachPS2KeyboardDevice( IN UNICODE_STRING* DeviceName, // ��Ҫ���ٵ��豸�� 
	IN PDRIVER_OBJECT DriverObject, // ��������Ҳ���Ǳ��������������� 
	OUT PDRIVER_OBJECT* FilterDriverObject ) // ���ظ��Ӻ���������� 
{ 
	PDEVICE_OBJECT DeviceObject; 
	PDEVICE_OBJECT FilterDeviceObject; 
	PDEVICE_OBJECT TargetDevice; 
	PFILE_OBJECT FileObject; 
	PDEVICE_EXTENSION DevExt; 
	NTSTATUS ntStatus; 
	// 
	// �����豸�����ҵ���Ҫ���ӵ��豸���� 
	// 
	ntStatus = IoGetDeviceObjectPointer( DeviceName, 
		FILE_ALL_ACCESS, 
		&FileObject, 
		&DeviceObject ); 
	if ( !NT_SUCCESS( ntStatus ) ) 
	{ 
		DbgPrint( "IoGetDeviceObjectPointer() 0x%x\n", ntStatus ); 
		return ntStatus; 
	} 
	DbgPrint( "Attached PS2 Keyboard Driver Name:%S,Driver Address:0x%x,DeviceAddress:0x%x\n", 
		DeviceObject->DriverObject->DriverName.Buffer, 
		DeviceObject->DriverObject,
		DeviceObject );
	// 
	// ���������豸���� 
	// 
	ntStatus = IoCreateDevice( DriverObject, 
		sizeof( DEVICE_EXTENSION ), 
		NULL, 
		FILE_DEVICE_KEYBOARD, 
		0, 
		FALSE, 
		&FilterDeviceObject ); 
	if ( !NT_SUCCESS( ntStatus ) ) 
	{ 
		ObDereferenceObject( FileObject ); 
		DbgPrint( "IoCreateDevice() 0x%x!\n", ntStatus ); 
		return ntStatus; 
	} 
	// 
	// �õ��豸��չ�ṹ���Ա����汣������豸��Ϣ 
	// 
	DevExt = ( PDEVICE_EXTENSION ) FilterDeviceObject->DeviceExtension; 
	// 
	// ��ʼ�������� 
	// 
	KeInitializeSpinLock( &DevExt->SpinLock ); 
	// 
	// ��ʼ�� IRP ������ 
	// 
	DevExt->IrpsInProgress = 0; 
	// 
	// �������豸���󸽼���Ŀ���豸����֮�ϣ������ظ��Ӻ��ԭ�豸���� 
	// 
	TargetDevice = IoAttachDeviceToDeviceStack( FilterDeviceObject, 
		DeviceObject ); 
	if ( !TargetDevice ) 
	{ 
		ObDereferenceObject( FileObject ); 
		IoDeleteDevice( FilterDeviceObject ); 
		DbgPrint( "IoAttachDeviceToDeviceStack() 0x%x!\n", ntStatus ); 
		return STATUS_INSUFFICIENT_RESOURCES; 
	} 
	// 
	// ��������豸��Ϣ 
	// 
	DevExt->DeviceObject = FilterDeviceObject; 
	DevExt->TargetDevice = TargetDevice; 
	DevExt->pFilterFileObject = FileObject; 
	// 
	// ���ù����豸�����Ϣ���־ 
	// 
	FilterDeviceObject->DeviceType = TargetDevice->DeviceType; 
	FilterDeviceObject->Characteristics = TargetDevice->Characteristics; 
	FilterDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING; 
	FilterDeviceObject->Flags |= ( TargetDevice->Flags &( DO_DIRECT_IO | 
		DO_BUFFERED_IO ) ); 
	// 
	// ���ظ��Ӻ���������� 
	// 
	*FilterDriverObject = TargetDevice->DriverObject; 
	ObDereferenceObject( FileObject ); 
	return STATUS_SUCCESS; 
} 
///////////////////////////////////////////////////////////////// 
// �������� : �Զ��幤�ߺ��� 
// ����ģ�� : ���̹���ģ�� 
//////////////////////////////////////////////////////////////// 
// ���� : ���̹��������� IRP_MJ_READ ��ǲ���̣����а��������� 
// ��� IRP ����� 
// ע�� : 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2007.2.15 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
//////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
NTSTATUS KeyReadPassThrough( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp ) 
{ 
	NTSTATUS status; 
	KIRQL IrqLevel; 
	PDEVICE_OBJECT pDeviceObject; 
	PDEVICE_EXTENSION KeyExtension = ( PDEVICE_EXTENSION ) 
		DeviceObject->DeviceExtension; 
	IoCopyCurrentIrpStackLocationToNext( Irp ); 
	// 
	// �� IRP ��������һ��Ϊ֧�� SMP ʹ�������� 
	// 
	KeAcquireSpinLock( &KeyExtension->SpinLock, &IrqLevel ); 
	InterlockedIncrement( &KeyExtension->IrpsInProgress ); 
	KeReleaseSpinLock( &KeyExtension->SpinLock, IrqLevel ); 
	IoSetCompletionRoutine( Irp, 
		KeyReadCompletion, 
		DeviceObject, 
		TRUE, 
		TRUE, 
		TRUE ); 
	return IoCallDriver( KeyExtension->TargetDevice, Irp ); 
} 
///////////////////////////////////////////////////////////////// 
// �������� ��ϵͳ�ص����� 
// ����ģ�� : ���̹���ģ�� 
//////////////////////////////////////////////////////////////// 
// ���� �� ��ü��̰���������Чɨ�����滻���Դﵽ���μ��̵�Ŀ�� 
// ע�� �� 
///////////////////////////////////////////////////////////////// 
// ���� : sinister 
// �����汾 : 1.00.00 
// �������� : 2007.2.12 
///////////////////////////////////////////////////////////////// 
// �� �� �� �� �� ʷ 
//////////////////////////////////////////////////////////////// 
// �޸��� : 
// �޸����� : 
// �޸����� : 
///////////////////////////////////////////////////////////////// 
NTSTATUS KeyReadCompletion( IN PDEVICE_OBJECT DeviceObject, 
	IN PIRP Irp, 
	IN PVOID Context ) 
{ 
	PIO_STACK_LOCATION IrpSp; 
	PKEYBOARD_INPUT_DATA KeyData; 
	PDEVICE_EXTENSION KeyExtension = ( PDEVICE_EXTENSION ) 
		DeviceObject->DeviceExtension; 
	int numKeys, i; 
	KIRQL IrqLevel; 
	IrpSp = IoGetCurrentIrpStackLocation( Irp ); 
	if ( Irp->IoStatus.Status != STATUS_SUCCESS ) 
	{ 
		DbgPrint( "ntStatus:0x%x", Irp->IoStatus.Status ); 
		goto __RoutineEnd; 
	} 
	// 
	// ϵͳ�� SystemBuffer �б��水����Ϣ 
	// 
	KeyData = Irp->AssociatedIrp.SystemBuffer; 
	if ( KeyData == NULL ) 
	{ 
		DbgPrint( "KeyData is NULL\n" ); 
		goto __RoutineEnd; 
	} 
	// 
	// �õ������� 
	// 
	numKeys = Irp->IoStatus.Information / sizeof( KEYBOARD_INPUT_DATA ); 
	if ( numKeys <0 ) 
	{ 
		DbgPrint( "numKeys less zero\n" ); 
		goto __RoutineEnd; 
	} 
	// 
	// ʹ�� 0 ��Чɨ�����滻���������а��� 
	// 
	for ( i = 0; i < numKeys; i++)
	{ 
		DbgPrint( "KeyDwon: 0x%x\n", KeyData[i].MakeCode ); 
			KeyData[i].MakeCode = 0x00; 
	} 
__RoutineEnd : 
	if ( Irp->PendingReturned ) 
	{ 
		IoMarkIrpPending( Irp ); 
	} 
	// 
	// �� IRP ��������һ��Ϊ֧�� SMP ʹ�������� 
	// 
	KeAcquireSpinLock( &KeyExtension->SpinLock, &IrqLevel ); 
	InterlockedDecrement( &KeyExtension->IrpsInProgress ); 
	KeReleaseSpinLock( &KeyExtension->SpinLock, IrqLevel ); 
	return Irp->IoStatus.Status ; 
}