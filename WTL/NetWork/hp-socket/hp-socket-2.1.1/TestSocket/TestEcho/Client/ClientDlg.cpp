
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include "WaitFor.h"


// CClientDlg dialog

#define DEFAULT_CONTENT	_T("text to be sent")
#define DEFAULT_ADDRESS	_T("127.0.0.1")
#define DEFAULT_PORT	_T("5555")


CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent), m_Client(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTENT, m_Content);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_ASYNC, m_Async);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_CONTENT, &CClientDlg::OnEnChangeContent)
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CClientDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_Content.SetWindowText(DEFAULT_CONTENT);
	m_Address.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_Async.SetCheck(BST_CHECKED);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPED);

	m_bAsyncConn = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (
			pMsg->message == WM_KEYDOWN		
			&&(	pMsg->wParam == VK_ESCAPE	 
			||	pMsg->wParam == VK_CANCEL	
			||	pMsg->wParam == VK_RETURN	
		))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CClientDlg::SetAppState(EnAppState state)
{
	m_enState = state;

	m_Async.EnableWindow(m_enState == ST_STOPED);
	m_Start.EnableWindow(m_enState == ST_STOPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED && m_Content.GetWindowTextLength() > 0);
	m_Address.EnableWindow(m_enState == ST_STOPED);
	m_Port.EnableWindow(m_enState == ST_STOPED);
}

void CClientDlg::OnEnChangeContent()
{
	m_Send.EnableWindow(m_enState == ST_STARTED && m_Content.GetWindowTextLength() > 0);
}


void CClientDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	CString strContent;
	m_Content.GetWindowText(strContent);

	LPSTR lpszContent = T2A((LPTSTR)(LPCTSTR)strContent);
	int iLen = (int)strlen(lpszContent);

	if(m_Client.Send(m_Client.GetConnectionID(), (LPBYTE)lpszContent, iLen))
		::LogSend(m_Client.GetConnectionID(), strContent);
	else
		::LogSendFail(m_Client.GetConnectionID(), m_Client.GetLastError(), m_Client.GetLastErrorDesc());
}


void CClientDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);

	CString strAddress;
	CString strPort;

	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);

	USHORT usPort	= (USHORT)_ttoi(strPort);
	m_bAsyncConn	= m_Async.GetCheck();

	::LogClientStarting(strAddress, usPort);

	if(m_Client.Start(strAddress, usPort, m_bAsyncConn))
	{

	}
	else
	{
		::LogClientStartFail(m_Client.GetLastError(), m_Client.GetLastErrorDesc());
		SetAppState(ST_STOPED);
	}
}


void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPING);

	if(m_Client.Stop())
	{
		::LogClientStop(m_Client.GetConnectionID());
		SetAppState(ST_STOPED);
	}
	else
	{
		ASSERT(FALSE);
	}
}

int CClientDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CClientDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

ISocketListener::EnHandleResult CClientDlg::OnConnect(DWORD dwConnectionID)
{
	CString strAddress;
	USHORT usPort;

	m_Client.GetLocalAddress(strAddress, usPort);

	::PostOnConnect(dwConnectionID, strAddress, usPort);
	SetAppState(ST_STARTED);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnSend(DWORD dwConnectionID, const BYTE* pData, int iLength)
{
	//static int t = 0;
	//if(++t % 3 == 0) return ISocketListener::HR_ERROR;

	::PostOnSend(dwConnectionID, pData, iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnReceive(DWORD dwConnectionID, const BYTE* pData, int iLength)
{
	//static int t = 0;
	//if(++t % 3 == 5) return ISocketListener::HR_ERROR;

	::PostOnReceive(dwConnectionID, pData, iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnClose(DWORD dwConnectionID)
{
	::PostOnClose(dwConnectionID);
	SetAppState(ST_STOPED);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnError(DWORD dwConnectionID, EnSocketOperation enOperation, int iErrorCode)
{
	::PostOnError(dwConnectionID, enOperation, iErrorCode);
	SetAppState(ST_STOPED);
	return ISocketListener::HR_OK;
}
