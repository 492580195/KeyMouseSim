// KeyMouseClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKeyMouseClientApp:
// �йش����ʵ�֣������ KeyMouseClient.cpp
//

class CKeyMouseClientApp : public CWinApp
{
public:
	CKeyMouseClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CKeyMouseClientApp theApp;