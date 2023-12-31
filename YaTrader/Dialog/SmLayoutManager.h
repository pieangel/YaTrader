#pragma once
#include <vector>
#include <map>
#include <string>
class CWnd;
namespace DarkHorse {
	class SmLayoutManager
	{
	public:
		SmLayoutManager(CWnd* parentWnd);
		~SmLayoutManager();
		void AddWindow(int resourceID);
		void AddWindow(std::string name, int resourceID, CRect rc);
		BOOL RedrawAll();
		CWnd* ParentWnd() const { return _ParentWnd; }
		void ParentWnd(CWnd* val) { _ParentWnd = val; }
		int GetWidth(int resourceID);
		int GetHeight(int resourceID);
		CRect& GetRect(int resourceID);
		void SetRect(int resourceID, CRect rcCtrl);
		std::map<int, std::pair<std::string, CRect>>& GetWndMap() {
			return _WndMap;
		}
		CRect GetClientArea(int resourceID);
	private:
		std::map<int, std::pair<std::string, CRect>> _WndMap;
		CWnd* _ParentWnd;
		std::vector<int> _WndIDVec;
		bool FindControl(int resourceID);
	};

}