// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__8AB81E68_CB2F_11D3_8D3B_AC2F34F1FA3C__INCLUDED_)
#define AFX_STDAFX_H__8AB81E68_CB2F_11D3_8D3B_AC2F34F1FA3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <commctrl.h>

#include <string>
#include <list>
#include <atlwin.h>
typedef std::list<std::basic_string<TCHAR> > string_list;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8AB81E68_CB2F_11D3_8D3B_AC2F34F1FA3C__INCLUDED)
