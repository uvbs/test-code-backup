#include "stdafx.h"
#include "CxIEWrapper.h"
#include "CxDocHostSite.h"

HWND CxIEWrapper::Create(HWND hWndParent, _U_RECT rect)
{
	HWND hWnd = NULL;

	hWnd = __super::Create(hWndParent, rect, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

	this->CreateWebBrowser(hWndParent, rect);

	this->UpdateWindow();
	this->ShowWindow(SW_SHOW);
	
	return hWnd;
}

BOOL CxIEWrapper::CreateWebBrowser(HWND hWndParent, _U_RECT rect)
{
	HRESULT hResult = E_FAIL;
	// 获取IE控件指针。
	hResult = this->CreateControl(L"{8856F961-340A-11D0-A96B-00C04FD705A2}", NULL);
	
	this->QueryControl(IID_IWebBrowser2, (void**)&m_spWeb);

	{
		CComPtr<IOleObject>	spIOleObject;

		hResult = m_spWeb->QueryInterface(IID_IOleObject, (void**)&spIOleObject);
		if (SUCCEEDED(hResult))
		{
			CxDocHostSite * _pCxDocHostSite = new CxDocHostSite;
			if (_pCxDocHostSite)
			{
				_pCxDocHostSite->SetParentWnd(hWndParent);
			
				// 设置自定义接口IOleClientSite，浏览器控件会调用IOleClientSite的QueryInterface接口。
				// 查询IDocHostUIHandler等接口！ by ZC. 2010-3-14 
				IOleClientSite *pIOleClientSite = _pCxDocHostSite;
				spIOleObject->SetClientSite(pIOleClientSite);

				spIOleObject->DoVerb(OLEIVERB_SHOW, NULL, pIOleClientSite, -1, hWndParent, rect.m_lpRect);
			}
		} // SUCCEEDED(hResult)
	}

	return (SUCCEEDED(hResult));
}

void CxIEWrapper::SetIDocHostUIHandler()
{
	
}

HRESULT CxIEWrapper::Navigate(LPCTSTR pszUrl)
{
	HRESULT hResult = E_FAIL;
	
	if (IsStringValid(pszUrl))
	{
		CComBSTR bstrUrl(pszUrl);

		hResult = m_spWeb->Navigate(bstrUrl, NULL, NULL, NULL, NULL);
	}

	return hResult;
}

