
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include "WaitFor.h"


// CClientDlg dialog

#define CONTENT_1		"X"
#define CONTENT_10		"0123456789"
#define DEFAULT_ADDRESS	_T("127.0.0.1")
#define DEFAULT_PORT	_T("5555")


CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_TEST_TIMES, m_TestTimes);
	DDX_Control(pDX, IDC_THR_COUNT, m_ThreadCount);
	DDX_Control(pDX, IDC_DATA_LEN, m_DataLen);
	DDX_Control(pDX, IDC_TEST_TIMES_INTERV, m_TestInterv);
	DDX_Control(pDX, IDC_THR_COUNT_INTERV, m_ThreadInterv);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
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

	m_TestTimes.SetCurSel(5);
	m_TestInterv.SetCurSel(1);
	m_ThreadCount.SetCurSel(5);
	m_ThreadInterv.SetCurSel(0);
	m_DataLen.SetCurSel(5);
	m_Address.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPED);

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
	if(m_enState == state)
		return;

	m_enState = state;

	m_Start.EnableWindow(m_enState == ST_STOPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Address.EnableWindow(m_enState == ST_STOPED);
	m_Port.EnableWindow(m_enState == ST_STOPED);
	m_TestTimes.EnableWindow(m_enState == ST_STOPED);
	m_TestInterv.EnableWindow(m_enState == ST_STOPED);
	m_ThreadCount.EnableWindow(m_enState == ST_STOPED);
	m_ThreadInterv.EnableWindow(m_enState == ST_STOPED);
	m_DataLen.EnableWindow(m_enState == ST_STOPED);
}

BOOL CClientDlg::CheckParams()
{
	BOOL isOK = TRUE;

	if(m_strAddress.IsEmpty())
	{
		m_Address.SetFocus();
		isOK = FALSE;
	}
	else if(m_usPort == 0)
	{
		m_Port.SetFocus();
		isOK = FALSE;
	}
	else if(m_iTestTimes <= 0)
	{
		m_TestTimes.SetFocus();
		isOK = FALSE;
	}
	else if(m_iTestInterv < 0)
	{
		m_TestInterv.SetFocus();
		isOK = FALSE;
	}
	else if(m_iThreadCount <= 0)
	{
		m_ThreadCount.SetFocus();
		isOK = FALSE;
	}
	else if(m_iThreadInterv < 0)
	{
		m_ThreadInterv.SetFocus();
		isOK = FALSE;
	}
	else if(m_iDataLen <= 0)
	{
		m_DataLen.SetFocus();
		isOK = FALSE;
	}

	if(!isOK)
		MessageBox(_T("One of settings is invalid, pls check!"), _T("Params Error"), MB_OK);

	return isOK;
}

void CClientDlg::OnBnClickedStart()
{
	CString strAddress;
	CString strPort;
	CString strTestTimes;
	CString strTestInterv;
	CString strThreadCount;
	CString strThreadInterv;
	CString strDataLen;

	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);
	m_TestTimes.GetWindowText(strTestTimes);
	m_TestInterv.GetWindowText(strTestInterv);
	m_ThreadCount.GetWindowText(strThreadCount);
	m_ThreadInterv.GetWindowText(strThreadInterv);
	m_DataLen.GetWindowText(strDataLen);

	m_strAddress	= strAddress.Trim();
	m_usPort		= (USHORT)_ttoi(strPort);
	m_iTestTimes	= _ttoi(strTestTimes);
	m_iTestInterv	= _ttoi(strTestInterv);
	m_iThreadCount	= _ttoi(strThreadCount);
	m_iThreadInterv	= _ttoi(strThreadInterv);
	m_iDataLen		= _ttoi(strDataLen);

	if(!CheckParams())
		return;

	SetAppState(ST_STARTING);

	m_dwBeginTickCount	= 0;
	m_dwEndTickCount	= 0;
	m_dwTimeconsuming	= 0;
	m_llTotalReceived	= 0;
	m_llTotalSent		= 0;
	m_llExpectReceived	= (LONGLONG)m_iTestTimes * (LONGLONG)m_iThreadCount * (LONGLONG)m_iDataLen;

	m_vtClients.Clear();
	m_sendBuffer.Malloc(m_iDataLen, true);

	for(int i = 0; i < m_iThreadCount; i++)
	{
		smart_simple_ptr<CSocketClient> pSocket = new CSocketClient(this);

		if(pSocket->Start(m_strAddress, m_usPort))
			m_vtClients->push_back(pSocket.release());
		else
		{
			::LogClientStartFail(pSocket->GetLastError(), pSocket->GetLastErrorDesc());
			m_vtClients.Clear();
			m_sendBuffer.Free();
			SetAppState(ST_STOPED);
			return;
		}
	}

	::LogClientStart(m_strAddress, m_usPort);

	DWORD dwSendDelay = 3;
	CString strMsg;

	strMsg.Format(_T(" *** willing to send data after %d seconds ..."), dwSendDelay);
	::LogMsg(strMsg);
	::WaitWithMessageLoop(dwSendDelay * 1000);

	SetAppState(ST_STARTED);

	m_dwBeginTickCount = ::GetTickCount();

	BOOL bTerminated = FALSE;
	for(int i = 0; !bTerminated && i < m_iTestTimes; i++)
	{
		for(int j = 0; !bTerminated && j < m_iThreadCount; j++)
		{
			CSocketClient* pSocket = m_vtClients[j];
			if(!pSocket->Send(pSocket->GetConnectionID(), m_sendBuffer, (int)m_sendBuffer.Size()))
			{
				::LogClientSendFail(i + 1, j + 1, pSocket->GetLastError(), pSocket->GetLastErrorDesc());
				bTerminated = TRUE;
				break;
			}

			if(m_iThreadInterv > 0 && j + 1 < m_iThreadCount)
				::WaitWithMessageLoop(m_iThreadInterv);
		}

		if(m_iTestInterv > 0 && i + 1 < m_iTestTimes)
			::WaitWithMessageLoop(m_iTestInterv);
	}
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPING);

	for(size_t i = 0; i < m_vtClients.Size(); i++)
	{
		CSocketClient* pSocket = m_vtClients[i];

		if(pSocket->Stop())
			::LogClientStop((DWORD)i + 1);
		else
			ASSERT(FALSE);
	}

	::WaitWithMessageLoop(100L);

	CString strMsg;
	strMsg.Format(	_T(" *** Summary: expect -  %lld, send - %lld, recv - %lld"),
					m_llExpectReceived, m_llTotalSent, m_llTotalReceived);

	::LogMsg(strMsg);

	if(m_llExpectReceived == m_llTotalSent && m_llTotalSent == m_llTotalReceived)
		strMsg.Format(_T(" *** Success: time consuming -  %u millisecond !"), m_dwTimeconsuming);
	else
		strMsg.Format(_T(" *** Fail: manual terminated ? (or data lost)"));

	::LogMsg(strMsg);

	SetAppState(ST_STOPED);
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

ISocketListener::EnHandleResult CClientDlg::OnPrepareSocket(DWORD dwConnID, SOCKET socket)
{
	//VERIFY(::SSO_SendBuffSize(socket, SO_MAX_MSG_SIZE / 4) == 0);
	//VERIFY(::SSO_RecvBuffSize(socket, SO_MAX_MSG_SIZE * 4) == 0);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnSend(DWORD dwConnectionID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnSend(dwConnectionID, pData, iLength);
#endif

#if (WINVER < 0x0502)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalSent, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalSent, iLength);
#endif

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnReceive(DWORD dwConnectionID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnReceive(dwConnectionID, pData, iLength);
#endif

#if (WINVER < 0x0502)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalReceived, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalReceived, iLength);
#endif

	if(m_llTotalReceived == m_llExpectReceived)
	{
		m_dwEndTickCount	= ::GetTickCount();
		m_dwTimeconsuming	= m_dwEndTickCount - m_dwBeginTickCount;

		::PostTimeConsuming(m_dwTimeconsuming);
	}

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

ISocketListener::EnHandleResult CClientDlg::OnConnect(DWORD dwConnectionID)
{
	::LogOnConnect2(dwConnectionID);

	return ISocketListener::HR_OK;
}
