#pragma once
#include <Windows.h>

#define MOUSE_LEFT_BUTTON_DOWN   0x0001  // Left Button changed to down.
#define MOUSE_LEFT_BUTTON_UP     0x0002  // Left Button changed to up.
#define MOUSE_RIGHT_BUTTON_DOWN  0x0004  // Right Button changed to down.
#define MOUSE_RIGHT_BUTTON_UP    0x0008  // Right Button changed to up.
#define MOUSE_MIDDLE_BUTTON_DOWN 0x0010  // Middle Button changed to down.
#define MOUSE_MIDDLE_BUTTON_UP   0x0020  // Middle Button changed to up.

#define KEY_MAKE  0	//����
#define KEY_BREAK 1	//�ɿ�
#define KEY_E0    2	//��չ��ʶ
#define KEY_E1    4

//��ȡȨ��
BOOL ImproveProcPriv(TCHAR* szPrivilegeName = NULL);
// ��ʼ��
BOOL InitializeWinKM(void);
// ж��
BOOL ShutdownWinKM(void);
//���������Ϣ
BOOL SendMouseOperate(ULONG ButtonFlags, ULONG dwX, ULONG dwY);
//���ͼ�����Ϣ
BOOL SendKeyOperate(ULONG uFlags, ULONG vKeyCode);

//��������
BOOL WinKMKeyDown(ULONG vKeyCoad);
//�����ɿ�
BOOL WinKMKeyUp(ULONG vKeyCoad);

//����������
BOOL WinKMLButtonDown();
//�������ɿ�
BOOL WinKMLButtonLUp();
//����Ҽ�����
BOOL WinKMRButtonDown();
//����Ҽ��ɿ�
BOOL WinKMRButtonLUp();
//����м�����
BOOL WinKMMButtonDown();
//����м��ɿ�
BOOL WinKMMButtonLUp();