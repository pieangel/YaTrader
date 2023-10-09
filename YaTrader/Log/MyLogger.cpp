#include "stdafx.h"
#include "WinSDKVer.h"
#include "MyLogger.h"
#include <sstream>
#include <filesystem>
#include <iostream>
#include <cstdint>
#include <iostream>
#include <cstring>
using namespace DarkHorse;

std::string CMyLogger::cur_log_file = "";

CMyLogger::CMyLogger()
{
	LogFileName = "Logging ";
	LogFileName = "";
	LogPath = "";
	m_loggableItem = static_cast<int>(LogItem::Function) |
		static_cast<int>(LogItem::Function) |
		static_cast<int>(LogItem::LineNumber) |
		static_cast<int>(LogItem::DateTime) |
		static_cast<int>(LogItem::LoggerName) |
		static_cast<int>(LogItem::LogLevel);


	_Info = false;
	_Debug = false;
	_Warn = false;
	_Error = true;

}


CMyLogger::~CMyLogger()
{
	Stop();
}

void CMyLogger::SuspendWorking(void)
{
	m_suspendMode = TRUE;
	this->suspend();
}

void CMyLogger::ResumeWorking(void)
{
	m_suspendMode = FALSE;
	this->resume();
}

void CMyLogger::Run()
{
	m_RunMode = true;
	start();
}
void CMyLogger::Stop()
{

	m_RunMode = false;
	m_Event.Set();
	stop();
	
}


void CMyLogger::loglevel_toString(LogLevel level, std::string &strLevel)
{
	strLevel = "";
	switch (level)
	{
	case LogLevel::Error:
		strLevel = "ERR";
		break;

	case LogLevel::Warn:
		strLevel = "WRN";
		break;

	case LogLevel::Info:
		strLevel = "INF";
		break;

	case LogLevel::Debug:
		strLevel = "DBG";
		break;
	}
}



void	CMyLogger::LoggingDataInfo(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...)
{
	if (_Info == false)		return;
	std::unique_lock<std::mutex> lock(log_mutex_info, std::try_to_lock);
	if (lock.owns_lock())
	{

		//int final_n, n = ((int)fmt.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		//if (n >= 4096)		return;	//write buff overflow	

		// read all len
		UINT32 buffLen = 0;
		{
			va_list ap;
			va_start(ap, fmt);
			buffLen = std::vsnprintf(0, 0, fmt.c_str(), ap);
			va_end(ap);
		}
		if (buffLen == 0)  return;
		buffLen++;
		char* pszBuffer;
		pszBuffer = new char[buffLen + 1];

		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf(pszBuffer, buffLen, fmt.c_str(), ap);
		va_end(ap);

		std::string strLevel;
		SYSTEMTIME st;
		GetLocalTime(&st);
		std::string cstrMessage = format_string("[%d-%02d-%02d %02d:%02d:%02d.%03d] %d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level);
		LoggDataPtr loggdataPtr = LoggDataPtr(new my_tupleLogger(cstrMessage, level, file, line, func, std::string(pszBuffer)));
		MessageQue.push(std::move(loggdataPtr));
		delete[] pszBuffer;
	}

}

void	CMyLogger::LoggingDataDebug(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...)
{

	if (_Debug == false)		return;
	std::unique_lock<std::mutex> lock(log_mutex_debug, std::try_to_lock);
	if (lock.owns_lock())
	{

		//int final_n, n = ((int)fmt.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		//if (n >= 4096)		return;	//write buff overflow	

		// read all len
		UINT32 buffLen = 0;
		{
			va_list ap;
			va_start(ap, fmt);
			buffLen = std::vsnprintf(0, 0, fmt.c_str(), ap);
			va_end(ap);
		}
		if (buffLen == 0)  return;
		buffLen++;
		char* pszBuffer;
		pszBuffer = new char[buffLen + 1];

		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf(pszBuffer, buffLen, fmt.c_str(), ap);
		va_end(ap);

		std::string strLevel;
		SYSTEMTIME st;
		GetSystemTime(&st);
		std::string cstrMessage = format_string("[%d-%02d-%02d %02d:%02d:%02d.%03d] %d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level);
		LoggDataPtr loggdataPtr = LoggDataPtr(new my_tupleLogger(cstrMessage, level, file, line, func, std::string(pszBuffer)));
		MessageQue.push(std::move(loggdataPtr));
		delete[] pszBuffer;
	}
}
void	CMyLogger::LoggingDataWarn(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...)
{
	
	if (_Warn == false)		return;
	std::unique_lock<std::mutex> lock(log_mutex_warn, std::try_to_lock);
	if (lock.owns_lock())
	{

		//int final_n, n = ((int)fmt.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		//if (n >= 4096)		return;	//write buff overflow	

		// read all len
		UINT32 buffLen = 0;
		{
			va_list ap;
			va_start(ap, fmt);
			buffLen = std::vsnprintf(0, 0, fmt.c_str(), ap);
			va_end(ap);
		}
		if (buffLen == 0)  return;
		buffLen++;
		char* pszBuffer;
		pszBuffer = new char[buffLen + 1];

		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf(pszBuffer, buffLen, fmt.c_str(), ap);
		va_end(ap);

		std::string strLevel;
		SYSTEMTIME st;
		GetSystemTime(&st);
		std::string cstrMessage = format_string("[%d-%02d-%02d %02d:%02d:%02d.%03d] %d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level);
		LoggDataPtr loggdataPtr = LoggDataPtr(new my_tupleLogger(cstrMessage, level, file, line, func, std::string(pszBuffer)));
		MessageQue.push(std::move(loggdataPtr));
		delete[] pszBuffer;
	}
}
void	CMyLogger::LoggingDataError(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...)
{

	if (_Error == false)		return;
	std::unique_lock<std::mutex> lock(log_mutex_error, std::try_to_lock);
	if (lock.owns_lock())
	{

		//int final_n, n = ((int)fmt.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		//if (n >= 4096)		return;	//write buff overflow	

		// read all len
		UINT32 buffLen = 0;
		{
			va_list ap;
			va_start(ap, fmt);
			buffLen = std::vsnprintf(0, 0, fmt.c_str(), ap);
			va_end(ap);
		}
		if (buffLen == 0)  return;
		buffLen++;
		char* pszBuffer;
		pszBuffer = new char[buffLen + 1];

		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf(pszBuffer, buffLen, fmt.c_str(), ap);
		va_end(ap);

		std::string strLevel;
		SYSTEMTIME st;
		GetSystemTime(&st);
		std::string cstrMessage = format_string("[%d-%02d-%02d %02d:%02d:%02d.%03d] %d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level);
		LoggDataPtr loggdataPtr = LoggDataPtr(new my_tupleLogger(cstrMessage, level, file, line, func, std::string(pszBuffer)));
		MessageQue.push(std::move(loggdataPtr));
		delete[] pszBuffer;
	}
}
//void	CMyLogger::LoggingData(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, LPCTSTR fmt, ...)

void	CMyLogger::LoggingData(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...)
{
	
	if (_Info == false && level == LogLevel::Info)			return;
	else if (_Debug == false && level == LogLevel::Debug)	return;
	else if (_Warn == false && level == LogLevel::Warn)		return;
	else if (_Error == false && level == LogLevel::Error)	return;


	std::unique_lock<std::mutex> lock(log_mutex, std::try_to_lock);
	if (lock.owns_lock())
	{

		//int final_n, n = ((int)fmt.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		//if (n >= 4096)		return;	//write buff overflow	

		// read all len
		UINT32 buffLen = 0;
		{
			va_list ap;
			va_start(ap, fmt);
			buffLen = std::vsnprintf(0, 0, fmt.c_str(), ap);
			va_end(ap);
		}
		if (buffLen == 0)  return;
		buffLen++;
		char* pszBuffer;
		pszBuffer = new char[buffLen + 1];

		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf(pszBuffer, buffLen, fmt.c_str(), ap);
		va_end(ap);

		std::string strLevel;
		SYSTEMTIME st;
		GetSystemTime(&st);
		std::string cstrMessage = format_string("[%d-%02d-%02d %02d:%02d:%02d.%03d] %d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level);
		LoggDataPtr loggdataPtr = LoggDataPtr(new my_tupleLogger(cstrMessage, level, file, line, func, std::string(pszBuffer)));
		MessageQue.push(std::move(loggdataPtr));
		delete[] pszBuffer;
	}

}
void CMyLogger::makeLogDir(std::string &makepath)
{
	//CStdioFile  fileOutput;
	//CFileException ex;

	namespace fs = std::filesystem;
	std::string workPath = makepath;

	fs::path strPath(workPath.c_str());

	if (!fs::exists(strPath))
	{
		//if (fs::create_directory(strPath) == false)
		if (fs::create_directories(strPath) == false)
		{
			//AfxMessageBox("Create Directory Error");
			return;
		}
	}
}
void CMyLogger::FileLog()
{
	std::ofstream writeFile;
	std::string cstrMessage, makepath;
	if (MessageQue.empty())  return;
	GetSystemTime(&logDay);
	cstrMessage = format_string("%s/%d%02d%02d/%s.log", LogPath.c_str(),logDay.wYear, logDay.wMonth, logDay.wDay, LogFileName.c_str());

	makepath = format_string("%s\\%d%02d%02d", LogPath.c_str(), logDay.wYear, logDay.wMonth, logDay.wDay);
	makeLogDir(makepath);
	cur_log_file = cstrMessage;
	writeFile.open(cstrMessage.c_str(), std::ios::out | std::ios::binary | std::ios::app);

	std::ostringstream sIstream;
	while (1)
	{
		if (MessageQue.empty())  break;
		LoggDataPtr loggdataPtr;
		
		loggdataPtr = MessageQue.pop();
		if (loggdataPtr != NULL)
		{
			//typedef std::tuple<std::string, LogLevel, std::string, INT, std::string, std::string> my_tupleLogger;
			
			auto [strMessage, level, file, line, func, formatInfo] = (*loggdataPtr);
			sIstream << strMessage.c_str();
			sIstream << " ";
			if (m_loggableItem & static_cast<int>(LogItem::Function))		sIstream << func.c_str();
			sIstream << " ";
			if (m_loggableItem & static_cast<int>(LogItem::Filename))	sIstream << file.c_str();
			sIstream << " ";
			if (m_loggableItem & static_cast<int>(LogItem::LineNumber)) sIstream << line;
			sIstream << " ";
			sIstream << formatInfo.c_str();
			sIstream << "\r\n";
			writeFile << sIstream.str();
			sIstream.str("");
		}
	}
	//<< std::flush;
	writeFile.close();

}
unsigned CMyLogger::ThreadHandlerProc()
{
	UINT watchCnt = 0;
	std::string chkMsg;
	std::string threadName;
	m_dwMilliseconds = 250;
	
	while (!m_RunMode) {
		/*auto StartTime = std::chrono::steady_clock::now();
		std::chrono::milliseconds dura_waitmSec(100);
		std::this_thread::sleep_until(StartTime + dura_waitmSec);*/    // Sleep(100)	run 될때 까지 준비
		Sleep(100);
	}

	while (1)
	{
		if (isStop() == true) break;
		watchCnt++;
		//std::cv_status dwStatus = m_Event.Wait(m_dwMilliseconds);
		DWORD dwStatus = m_Event.Wait(m_dwMilliseconds);
		switch (dwStatus)
		{
		case WAIT_TIMEOUT:			//The time-out interval elapsed, and the object's state is nonsignaled.
			if (MessageQue.empty() != true)
			{
				FileLog();
			}
			break;
		
		}
	}
	return 0;
}