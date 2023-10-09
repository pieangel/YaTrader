#include "stdafx.h"
#include "SmLayoutManager.h"

namespace DarkHorse {
	SmLayoutManager::SmLayoutManager(CWnd* parentWnd)
	{
		_ParentWnd = parentWnd;
	}


	SmLayoutManager::~SmLayoutManager()
	{
	}

	void SmLayoutManager::AddWindow(int resourceID)
	{
		if (!_ParentWnd)
			return;

		_WndIDVec.push_back(resourceID);
	}

	void SmLayoutManager::AddWindow(std::string name, int resourceID, CRect rc)
	{
		_WndMap[resourceID] = std::make_pair(name, rc);
	}

	BOOL SmLayoutManager::RedrawAll()
	{
		if (!_ParentWnd)
			return FALSE;

		HDWP hdwp = ::BeginDeferWindowPos(_WndMap.size());
		if (hdwp) {
			for (auto it = _WndMap.begin(); it != _WndMap.end(); ++it) {
				std::pair<std::string, CRect> item = it->second;
				CWnd* pWnd = (CWnd*)::GetDlgItem(_ParentWnd->GetSafeHwnd(), it->first);
				if (!pWnd || !pWnd->GetSafeHwnd()) continue;
				if (pWnd->GetSafeHwnd()) {
					CRect rectWC = std::get<1>(item);
					::DeferWindowPos(hdwp, pWnd->m_hWnd, NULL,
						rectWC.left, rectWC.top, rectWC.Width(), rectWC.Height(),
						SWP_NOZORDER);
					pWnd->RedrawWindow();

				}
			}
			return ::EndDeferWindowPos(hdwp);
		}

		return FALSE;
	}

	int SmLayoutManager::GetWidth(int resourceID)
	{
		auto it = _WndMap.find(resourceID);
		if (it != _WndMap.end()) {
			return std::get<1>(it->second).Width();
		}
		else {
			return -1;
		}
	}

	int SmLayoutManager::GetHeight(int resourceID)
	{
		auto it = _WndMap.find(resourceID);
		if (it != _WndMap.end()) {
			return std::get<1>(it->second).Height();
		}
		else {
			return -1;
		}
	}

	CRect& SmLayoutManager::GetRect(int resourceID)
	{
		std::pair<std::string, CRect>& item = _WndMap[resourceID];
		return std::get<1>(item);
	}

	void SmLayoutManager::SetRect(int resourceID, CRect rcCtrl)
	{
		if (!FindControl(resourceID))
			return;

		std::pair<std::string, CRect>& item = _WndMap[resourceID];
		CRect& rcWnd = std::get<1>(item);
		rcWnd.left = rcCtrl.left;
		rcWnd.top = rcCtrl.top;
		rcWnd.right = rcCtrl.right;
		rcWnd.bottom = rcCtrl.bottom;
	}

	CRect SmLayoutManager::GetClientArea(int resourceID)
	{
		CRect rcWnd;
		if (!_ParentWnd)
			return rcWnd;

		CWnd* wnd = (CWnd*)_ParentWnd->GetDlgItem(resourceID);
		wnd->GetWindowRect(rcWnd);
		_ParentWnd->ScreenToClient(rcWnd);

		return rcWnd;
	}

	bool SmLayoutManager::FindControl(int resourceID)
	{
		auto it = _WndMap.find(resourceID);
		if (it != _WndMap.end()) {
			return true;
		}
		else {
			return false;
		}
	}

}