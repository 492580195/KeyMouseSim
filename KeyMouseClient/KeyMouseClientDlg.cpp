// KeyMouseClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KeyMouseClient.h"
#include "KeyMouseClientDlg.h"
#include "WinKM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()


// CKeyMouseClientDlg �Ի���




CKeyMouseClientDlg::CKeyMouseClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyMouseClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyMouseClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKeyMouseClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP	
	ON_WM_CLOSE()	
	ON_BN_CLICKED(IDC_BUTTON_START, &CKeyMouseClientDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PROCESS_OFF, &CKeyMouseClientDlg::OnBnClickedButtonProcessOff)
END_MESSAGE_MAP()

// CKeyMouseClientDlg ��Ϣ�������

BOOL CKeyMouseClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	if(InitializeWinKM())
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("�������سɹ�");
	else
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("��������ʧ��");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CKeyMouseClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CKeyMouseClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CKeyMouseClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKeyMouseClientDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ShutdownWinKM();
	CDialog::OnClose();
}




void CKeyMouseClientDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	//KeyOpeate(VK_F1, FALSE);
	//KeyOpeate(VK_F1, TRUE);

// 	DWORD KeyCode = MapVirtualKey(VK_LWIN, 0);	//��ȡɨ����
// 	CString strTemp;
// 	strTemp.Format("%x", KeyCode);
// 	MessageBox(strTemp);
/*	
	SetCursorPos(100, 100);
	MouseOpeate(0, 0, 0);
	MouseOpeate(1, 0, 0);
	MouseOpeate(0, 0, 0);
	MouseOpeate(1, 0, 0);
	Sleep(1000);
	MouseOpeate(2, 0, 0);
	MouseOpeate(3, 0, 0);

	SetCursorPos(100, 100);
	SendMouseOperate(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
	SendMouseOperate(MOUSE_LEFT_BUTTON_UP, 0, 0);
	SendMouseOperate(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
	SendMouseOperate(MOUSE_LEFT_BUTTON_UP, 0, 0);
	Sleep(1000);
	SendMouseOperate(MOUSE_RIGHT_BUTTON_DOWN, 0, 0);
	SendMouseOperate(MOUSE_RIGHT_BUTTON_UP, 0, 0);
*/
	//SendKeyOperate(KEY_MAKE, VK_LWIN);
	//SendKeyOperate(KEY_BREAK, VK_LWIN);	

// 	SetCursorPos(200, 200);
// 	SendMouseOperate(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
// 	SendMouseOperate(MOUSE_LEFT_BUTTON_UP, 0, 0);
// 	SendKeyOperate(KEY_MAKE, VK_CONTROL);
// 	SendKeyOperate(KEY_MAKE, 'A');	
// 	SendKeyOperate(KEY_BREAK, 'A');
// 	SendKeyOperate(KEY_BREAK, VK_CONTROL);
	SetTimer(100, 1000, NULL);
}


void CKeyMouseClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
// 	SendMouseOperate(MOUSE_LEFT_BUTTON_DOWN, 0, 0);
// 	Sleep(10);
// 	SendMouseOperate(MOUSE_LEFT_BUTTON_UP, 0, 0);
// 	SendKeyOperate(KEY_MAKE, VK_RETURN);
// 	SendKeyOperate(KEY_BREAK, VK_RETURN);
	WinKMLButtonDown();
	Sleep(10);
	WinKMLButtonLUp();
	WinKMKeyDown(VK_RETURN);
	WinKMKeyUp(VK_RETURN);
	CDialog::OnTimer(nIDEvent);
}


void CKeyMouseClientDlg::OnBnClickedButtonProcessOff()
{
	// TODO: Add your control notification handler code here
	KillTimer(100);
}
