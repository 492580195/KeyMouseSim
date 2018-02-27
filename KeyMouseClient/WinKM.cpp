#include "WinKM.h"
#include "DriverManage.h"

BOOL ImproveProcPriv(TCHAR* szPrivilegeName)
{
	HANDLE token;
	//����Ȩ��
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&token))
	{
		//MessageBox(NULL,"�򿪽�������ʧ��...","����",MB_ICONSTOP);
		return FALSE;
	}
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	//::LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid); // ��� SE_DEBUG_NAME ��Ȩ
	if(szPrivilegeName)
		::LookupPrivilegeValue(NULL,szPrivilegeName,&tkp.Privileges[0].Luid);
	else
		::LookupPrivilegeValue(NULL,SE_LOAD_DRIVER_NAME,&tkp.Privileges[0].Luid);
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(token,FALSE,&tkp,sizeof(tkp),NULL,NULL))
	{
		//MessageBox(NULL,"��������Ȩ��ʧ��...","����",MB_ICONSTOP);
		return FALSE;
	}
	CloseHandle(token);
	return TRUE;
}

// ��ʼ��
BOOL InitializeWinKM(void)
{
	ImproveProcPriv();			
	return g_DriverManage.StartDriver();
}

// ж��
BOOL ShutdownWinKM(void)
{
	return g_DriverManage.StopDriver();
}
//���������Ϣ
BOOL SendMouseOperate(ULONG ButtonFlags, ULONG dwX, ULONG dwY)
{
	ULONG dwMouseData[3];
	dwMouseData[0] = ButtonFlags;
	dwMouseData[1] = dwX;
	dwMouseData[2] = dwY;
	return g_DriverManage.SendDeviceControl(IOCTL_SEND_MOUSE, &dwMouseData, sizeof(ULONG)*3, 0, NULL);
}
//���ͼ�����Ϣ
BOOL SendKeyOperate(ULONG uFlags, ULONG vKeyCode)
{
	ULONG uMakeCode = MapVirtualKey(vKeyCode, 0);	//��ȡɨ����
	switch(vKeyCode)
	{
	case VK_INSERT:	
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:	//Page Up
	case VK_NEXT:	//Page Down

	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:

	case VK_DIVIDE:

	case VK_LWIN:
	case VK_RCONTROL:
	case VK_RWIN:
	case VK_RMENU:	//ALT
		uFlags |= KEY_E0;
		break;
		//case VK_PAUSE:
		//	uFlags |= KEY_E1;
	}

	ULONG dwKeyData[2];
	dwKeyData[0] = uFlags;
	dwKeyData[1] = uMakeCode;
	return g_DriverManage.SendDeviceControl(IOCTL_SEND_KEY, &dwKeyData, sizeof(ULONG)*2, 0, NULL);
}

//��������
BOOL WinKMKeyDown(ULONG vKeyCoad)
{
	return SendKeyOperate(KEY_MAKE, vKeyCoad);
	
}
//�����ɿ�
BOOL WinKMKeyUp(ULONG vKeyCoad)
{
	return SendKeyOperate(KEY_BREAK, vKeyCoad);
}
//����������
BOOL WinKMLButtonDown()
{
	return SendMouseOperate(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
}
//�������ɿ�
BOOL WinKMLButtonLUp()
{
	return SendMouseOperate(MOUSE_LEFT_BUTTON_UP, 0, 0);
}
//����Ҽ�����
BOOL WinKMRButtonDown()
{
	return SendMouseOperate(MOUSE_RIGHT_BUTTON_DOWN, 0, 0);
}
//����Ҽ��ɿ�
BOOL WinKMRButtonLUp()
{
	return SendMouseOperate(MOUSE_RIGHT_BUTTON_UP, 0, 0);
}
//����м�����
BOOL WinKMMButtonDown()
{
	return SendMouseOperate(MOUSE_MIDDLE_BUTTON_DOWN, 0, 0);
}
//����м��ɿ�
BOOL WinKMMButtonLUp()
{
	return SendMouseOperate(MOUSE_MIDDLE_BUTTON_UP, 0, 0);
}