
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "socket/IocpServer.h"
#include "helper.h"

// CServerDlg dialog
class CServerDlg : public CDialogEx, public CServerSocketListener
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
	void Statistics();
	void Reset(BOOL bResetClientCount = TRUE);
private:
	virtual ISocketListener::EnHandleResult OnPrepareListen(SOCKET soListen);
	virtual ISocketListener::EnHandleResult OnSend(DWORD dwConnectionID, const BYTE* pData, int iLength);
	virtual ISocketListener::EnHandleResult OnReceive(DWORD dwConnectionID, const BYTE* pData, int iLength);
	virtual ISocketListener::EnHandleResult OnClose(DWORD dwConnectionID);
	virtual ISocketListener::EnHandleResult OnError(DWORD dwConnectionID, EnSocketOperation enOperation, int iErrorCode);
	virtual ISocketListener::EnHandleResult OnAccept(DWORD dwConnectionID, SOCKET soClient);
	virtual ISocketListener::EnHandleResult OnServerShutdown();

private:
	CListBox m_Info;
	CButton m_Start;
	CButton m_Stop;
	CIocpServer m_Server;
	EnAppState m_enState;

	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;
	volatile LONG m_lClientCount;

	CCriSec m_cs;

private:
	static const USHORT PORT;
	static const LPCTSTR ADDRESS;
};
