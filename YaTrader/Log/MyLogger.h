#pragma once
#include <windows.h>
#include "../Common/common.h"
#include<iostream>
#include<fstream>
#include <tuple>
#include "../Common/Queue.h"
namespace DarkHorse {

	enum class LogLevel
	{
		Info = 1,
		Debug,
		Warn,
		Error
	};
	enum class LogItem
	{
		Filename = 0x1,
		LineNumber = 0x2,
		Function = 0x4,
		DateTime = 0x8,
		ThreadId = 0x10,
		LoggerName = 0x20,
		LogLevel = 0x40
	};
#define MAX_TEXT_BUFFER_SIZE 5120

#define WRITELOG(logObj, level, fmt, ...)		logObj.LoggingData(level, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGINFO(logObj, fmt, ...)				logObj.LoggingDataInfo(LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGDEBUG(logObj, fmt, ...)				logObj.LoggingDataDebug(LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGWARN(logObj, fmt, ...)				logObj.LoggingDataWarn(LogLevel::Warn, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGERROR(logObj, fmt, ...)				logObj.LoggingDataError(LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);

	//#define WRITELOGP(logObjPtr, level, fmt, ...)	if(logObjPtr != NULL) logObjPtr->LoggingData(level, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGINFOP(logObjPtr, fmt, ...)			if(logObjPtr != NULL) logObjPtr->LoggingDataInfo(LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGDEBUGP(logObjPtr, fmt, ...)			if(logObjPtr != NULL) logObjPtr->LoggingDataDebug(LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGWARNP(logObjPtr, fmt, ...)			if(logObjPtr != NULL) logObjPtr->LoggingDataWarn(LogLevel::Warn, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LOGERRORP(logObjPtr, fmt, ...)			if(logObjPtr != NULL) logObjPtr->LoggingDataError(LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);

	class CMyLogger : public Runnable
	{
	public:
		CMyLogger();
		~CMyLogger();
		// singletone
		CMyLogger(const CMyLogger&) = delete;
		CMyLogger& operator=(const CMyLogger&) = delete;
		static CMyLogger& getInstance()
		{
			static CMyLogger so;
			return so;
		};
		static std::string cur_log_file;
	public:
		void	SuspendWorking(void);
		void	ResumeWorking(void);
		void	Run();
		void	Stop();

		void	LoggingDataInfo(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...);
		void	LoggingDataDebug(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...);
		void	LoggingDataWarn(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...);
		void	LoggingDataError(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...);

		void	LoggingData(LogLevel level, LPCTSTR file, INT line, LPCTSTR func, std::string fmt, ...);
		void    setLogFile(char* filepath, const char* fileName)
		{
			LogFileName = fileName;
			LogPath = filepath;
		};

		void SetLoggerInfo(bool param)
		{
			_Info = param;
		};
		void SetLoggerDebug(bool param)
		{
			_Debug = param;
		};
		void SetLoggerWarn(bool param)
		{
			_Warn = param;
		};
		void SetLoggerError(bool param)
		{
			_Error = param;
		};


	private:
		unsigned ThreadHandlerProc(void);
		void loglevel_toString(LogLevel level, std::string& strLevel);
		void FileLog();
		void makeLogDir(std::string& makepath);


	private:

		typedef std::tuple<std::string, LogLevel, std::string, INT, std::string, std::string> my_tupleLogger;
		using LoggDataPtr = std::shared_ptr<my_tupleLogger>;


		SYSTEMTIME logDay;
		//	threadsafe_queue<my_tupleLogger> MessageQue;
		threadsafe_queue<LoggDataPtr> MessageQue;


		std::atomic<bool>m_RunMode;
		int m_loggableItem;
		std::atomic<BOOL>ExitCode;
		std::atomic<BOOL>OpenFlag;
		std::atomic<BOOL> m_suspendMode;
		CMyEvent m_Event;
		DWORD m_dwMilliseconds;
		std::mutex log_mutex_info;
		std::mutex log_mutex_debug;
		std::mutex log_mutex_warn;
		std::mutex log_mutex_error;
		std::mutex log_mutex;
		std::string LogFileName;
		std::string LogPath;

		bool _Info;
		bool _Debug;
		bool _Warn;
		bool _Error;


	};

}
