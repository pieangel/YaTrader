#pragma once
#include <string>
namespace DarkHorse {
	class IProgressDialog
	{
	public:
		virtual void SetTotalCount(const int& count) = 0;
		virtual void SetRemainCount(const int& count) = 0;
		virtual void SetTaskTitle(const std::string& title) = 0;
		virtual void SetTaskDetail(const std::string& detail) = 0;
		virtual void RefreshProgress() = 0;
		virtual void ShowHide(const bool show) = 0;
		virtual bool GetShowState() const = 0;
	};
}

