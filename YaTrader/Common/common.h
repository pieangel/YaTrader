#pragma once
#include <map>
#include <vector>
#include <list>
#include <condition_variable>
#include <mutex>
#include <string>
#include <windows.h>
//#include <Mmsystem.h>
#include <sstream>


// Remark Begine [2017-8-29  Debug by mnOh]
//
#include <atomic>
#include <thread>
namespace DarkHorse {

	class Runnable
	{
	public:
		Runnable() : m_stop(), m_thread() { m_runMode = false; }
		virtual ~Runnable() { }

		Runnable(Runnable const&) = delete;
		Runnable& operator = (Runnable const&) = delete;

		bool isRun() { return m_runMode; }

	protected:
		void start() { if (m_runMode != true) { m_runMode = true;  m_thread = std::thread(&Runnable::ThreadHandlerProc, this); } }
		void stop() { m_stop = true; if (m_runMode == true) m_thread.join(); m_runMode = false; }
		void suspend() { SuspendThread(m_thread.native_handle()); }
		void resume() { ResumeThread(m_thread.native_handle()); }
		bool isStop() { return m_stop; }
		void setpriority(int currentPriority) { SetThreadPriority(m_thread.native_handle(), currentPriority); }


	protected:
		//	virtual void run() = 0;
		virtual unsigned int ThreadHandlerProc(void) = 0;
	protected:
		std::atomic<bool> m_stop;
		std::atomic<bool> m_runMode;
		std::thread m_thread;
	};
	
	class CMyEvent
	{
	public:

		CMyEvent(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL)
		{
			m_hHandle = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
			if (MyIsValidHandle(m_hHandle))
			{
				if (lpName)
					m_dwStatus = GetLastError();
				else
					m_dwStatus = NO_ERROR;
			}
			else
			{
				m_dwStatus = GetLastError();
				//ThrowError(m_dwStatus);
			}
		}

		// constructor opens an existing named event...
		CMyEvent(LPCTSTR lpName, BOOL bInheritHandle, DWORD dwDesiredAccess)
		{
			m_hHandle = ::OpenEvent(dwDesiredAccess, bInheritHandle, lpName);
			if (MyIsValidHandle(m_hHandle)) m_dwStatus = NO_ERROR;
			else							m_dwStatus = GetLastError();
		}

		// operations on event object...
		BOOL Set(void)
		{
			return ::SetEvent(m_hHandle);
		}

		BOOL Reset(void)
		{
			return ::ResetEvent(m_hHandle);
		}

		BOOL Pulse(void)
		{
			return ::PulseEvent(m_hHandle);
		}
		DWORD Status(void) const
		{
			return m_dwStatus;
		}

		DWORD Wait(DWORD dwMilliseconds)
		{
			return ::WaitForSingleObject(m_hHandle, dwMilliseconds);
		}
	private:
		inline BOOL MyIsValidHandle(HANDLE hHandle)
		{
			return ((hHandle != NULL) && (hHandle != INVALID_HANDLE_VALUE));
		}
	protected:
		HANDLE m_hHandle;
		DWORD m_dwStatus;
	};




	template<typename ... Args>
	std::string format_string(const std::string& format, Args ... args)
	{

		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size <= 0) { throw std::runtime_error("Error during formatting."); }
		std::unique_ptr<char[]> buffer(new char[size]);
		snprintf(buffer.get(), size, format.c_str(), args ...);
		return std::string(buffer.get(), buffer.get() + size - 1);

	}


	//std::vector<std::string> line_vector = split(text, '$');
	template<typename ... Args>
	std::vector<std::string> split(std::string& str, char delimiter)
	{
		std::vector<std::string> _internal;
		std::stringstream ss(str);
		std::string temp;

		while (std::getline(ss, temp, delimiter))
		{
			_internal.push_back(temp);
		}

		return _internal;
	}

	template<typename ... Args>
	int findSubStr(std::string& str, std::string& substr)
	{
		size_t n = str.find(substr);
		if (n == std::string::npos) return -1;
		return (int)n;
	}

	typedef struct _sendDataStruct_ {
		DWORD	dwData;
		DWORD	cbData;
		char	svrName[256];
	}sendDataStruct;

}