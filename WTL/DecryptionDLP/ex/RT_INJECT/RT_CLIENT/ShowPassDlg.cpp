// ShowPassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowPass.h"
#include "ShowPassDlg.h"
//#include <Psapi.h>
#include "Psapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowPassDlg dialog

CShowPassDlg::CShowPassDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CShowPassDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CShowPassDlg)
    m_x = 0;
    m_y = 0;
    m_h = 20;
    m_w = 80;
    m_szDllName = _T("d:\\wndproc_dll.dll");
    m_szBtnName = _T("Welcome");
    m_uBtnID = 0x7359;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_bFind = FALSE;

    m_hFindCur = NULL;

    m_hPreWnd = NULL;

    m_hwndOther = 0;
}

CShowPassDlg::~CShowPassDlg()
{
    if (m_hFindCur)
    {
        DestroyCursor(m_hFindCur);
    }
    if (m_hFindIcon)
    {
        DeleteObject(m_hFindIcon);
    }
}

void CShowPassDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CShowPassDlg)
    DDX_Text(pDX, IDC_EDIT_X, m_x);
    DDX_Text(pDX, IDC_EDIT_Y, m_y);
    DDX_Text(pDX, IDC_EDIT_H, m_h);
    DDX_Text(pDX, IDC_EDIT_W, m_w);
    DDX_Text(pDX, IDC_EDIT_DLLFILE, m_szDllName);
    DDX_Text(pDX, IDC_EDIT_BTNNAME, m_szBtnName);
    DDX_Text(pDX, IDC_EDIT_BTNID, m_uBtnID);
    //}}AFX_DATA_MAP
}

#define UM_GETPASS WM_USER + 0x392

BEGIN_MESSAGE_MAP(CShowPassDlg, CDialog)
    //{{AFX_MSG_MAP(CShowPassDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_COPYDATA()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_BUTTON_CREATEBTN, OnButtonCreatebtn)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowPassDlg message handlers

BOOL CShowPassDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
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

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    if (!DoInit())
    {
        MessageBox(_T("程序初始化失败"), _T("信息"), MB_OK | MB_ICONINFORMATION);
        EndDialog(0);
    }
    CStatic *img = (CStatic *)GetDlgItem(IDC_STATIC_IMG);
    if (img)
    {
        int cx = GetSystemMetrics(SM_CXCURSOR);
        int cy = GetSystemMetrics(SM_CXCURSOR);

        m_hFindIcon = AfxGetApp()->LoadIcon(IDI_ICON_FIND);

        /*		m_hFindCur  = (HCURSOR)LoadImage(AfxGetInstanceHandle(),
        									MAKEINTRESOURCE(IDC_LOOK_CUR), IMAGE_CURSOR,
        									cx, cy,	LR_DEFAULTCOLOR);
        */
        GetIconInfo(m_hFindIcon, &m_ii);
        ICONINFO ii;
        ii.fIcon = FALSE;
        ii.xHotspot = cx / 2;
        ii.yHotspot = cy / 2;
        ii.hbmColor = m_ii.hbmColor;
        ii.hbmMask  = m_ii.hbmMask;
        m_hFindCur  = (HCURSOR)CreateIconIndirect(&ii);
        m_hOldBmp = img->SetBitmap(m_ii.hbmColor);
    }
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CShowPassDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_CLOSE)
    {
        DoClose();
    }
    else if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShowPassDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CShowPassDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}


BOOL CShowPassDlg::OnCopyData(CWnd *pWnd, COPYDATASTRUCT *pCopyDataStruct)
{
    // TODO: Add your message handler code here and/or call default
    SetDlgItemText(IDC_EDIT_PASS, (LPCTSTR)pCopyDataStruct->lpData);
    return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CShowPassDlg::OnCancel()
{
    // TODO: Add extra cleanup here

    //	CDialog::OnCancel();
}

void CShowPassDlg::OnOK()
{
    // TODO: Add extra validation here

    //	CDialog::OnOK();
}

void CShowPassDlg::DoClose()
{
    ReleaseHook();
    EndDialog(0);
}

BOOL CShowPassDlg::DoInit()
{
    RECT r;
    GetWindowRect(&r);
    SetWindowPos(&wndTopMost, r.left, r.top, r.right, r.bottom, SWP_NOOWNERZORDER);
    return InitHook(m_hWnd);
}


void CShowPassDlg::StartFind()
{
    CStatic *img = (CStatic *)GetDlgItem(IDC_STATIC_IMG);
    if (img)
    {
        m_bFind = TRUE;
        img->SetBitmap(m_hOldBmp);
        //	::SetClassLong(m_hWnd, GCL_HCURSOR,	(LONG)m_hFindCur);
        m_hDefCur = ::SetCursor(m_hFindCur);
        ::SetCapture(m_hWnd);
    }

}

void CShowPassDlg::EndFind()
{
    CStatic *img = (CStatic *)GetDlgItem(IDC_STATIC_IMG);
    if (img)
    {
        m_bFind = FALSE;
        img->SetBitmap(m_ii.hbmColor);
        ::ReleaseCapture();
        //		::SetClassLong(m_hWnd, GCL_HCURSOR,	(LONG)m_hDefCur);
        ::SetCursor(m_hDefCur);
        if (m_hPreWnd)
        {
            DrawBorder(m_hPreWnd);
        }
    }
}

BOOL CShowPassDlg::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(m_bFind && ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) || pMsg->message == WM_LBUTTONUP))
    {
        EndFind();
        return TRUE;
    }
    else
        return CDialog::PreTranslateMessage(pMsg);
}

void CShowPassDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CStatic *img = (CStatic *)GetDlgItem(IDC_STATIC_IMG);
    if (img)
    {
        CRect r;
        img->GetWindowRect(&r);
        ScreenToClient(&r);
        if (r.PtInRect(point))
        {
            StartFind();
        }
    }
    CDialog::OnLButtonDown(nFlags, point);
}

void CShowPassDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_bFind)
    {
        ClientToScreen(&point);
        HWND hWnd = GetWindowFromPoint(point);

        if (hWnd)
        {
            DWORD pid;
            GetWindowThreadProcessId(hWnd, &pid);

            if (pid != GetCurrentProcessId())
            {
                if (hWnd != m_hPreWnd)
                {
                    DrawBorder(m_hPreWnd);
                    m_hPreWnd = hWnd;
                    DrawBorder(m_hPreWnd);
                    GetPassText(hWnd, m_hWnd);

                    TCHAR c[MAX_PATH + 1];

                    m_hwndOther = hWnd;
                    wsprintf(c, _T("%08x"), hWnd);
                    SetDlgItemText(IDC_STATIC_HWND, c);

                    wsprintf(c, _T("%08x"), ::GetWindowLong(hWnd, GWL_ID));
                    SetDlgItemText(IDC_STATIC_ID, c);

                    GetClassName(hWnd, c, MAX_PATH);
                    SetDlgItemText(IDC_STATIC_CLASS, c);

                    if (GetAppName(hWnd, c, MAX_PATH))
                    {
                        SetDlgItemText(IDC_EDIT_FILE, c);
                    }
                }
            }
        }
    }
    CDialog::OnMouseMove(nFlags, point);
}

BOOL CShowPassDlg::GetAppName(HWND hWnd, LPSTR szAppName, int n)
{
    OSVERSIONINFO oi;
    BOOL brt = FALSE;
    DWORD pid;

    ZeroMemory(&oi, sizeof(oi));
    oi.dwOSVersionInfoSize = sizeof(oi);
    if (GetVersionEx(&oi))
    {
        if (oi.dwPlatformId == VER_PLATFORM_WIN32_NT)
        {
            GetWindowThreadProcessId(hWnd, &pid);
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
            if (hProcess)
            {
                HINSTANCE hIns = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
                if (hIns)
                {
                    //					GetModuleFileNameEx(hProcess, hIns, szAppName, n);
                    brt = FALSE;
                }
                CloseHandle(hProcess);
            }
        }
        else
        {
            //			GetWindowModuleFileName(hWnd, szAppName, n);
            brt = TRUE;
        }
    }

    return brt;
}



void CShowPassDlg::OnButtonCreatebtn()
{
    // TODO: Add your control notification handler code here

    if (m_hwndOther)
    {
        UpdateData(TRUE);
        RECT r;
        SetRect(&r, m_x, m_y, m_w + m_x, m_h + m_y);
        if (!RT_CTRL_BTN(m_szDllName, m_hwndOther, m_uBtnID, &r, m_szBtnName))
        {
            MessageBox("create btn failed", "xixi", MB_OK | MB_ICONINFORMATION);
        }
    }
}

