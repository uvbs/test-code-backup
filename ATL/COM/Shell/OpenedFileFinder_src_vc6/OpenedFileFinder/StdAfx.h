// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6566AC22_2048_468B_8408_201ABDEFC9A5__INCLUDED_)
#define AFX_STDAFX_H__6566AC22_2048_468B_8408_201ABDEFC9A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ISOLATION_AWARE_ENABLED 1
#define SIDEBYSIDE_COMMONCONTROLS 1

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include "commctrl.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6566AC22_2048_468B_8408_201ABDEFC9A5__INCLUDED)
