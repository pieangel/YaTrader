
#pragma once
#pragma warning( disable : 4996 )
#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <list>
#include <iostream>
#include <crtdbg.h>
#include <process.h>
#include "ReadDirectoryChanges.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include "../Log/MyLogger.h"
#include "../Json/json.hpp"
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSystemManager.h"

using namespace nlohmann;
//#include "VtOutSystemOrderManager.h"
//#include "SmFileSignalManager.h"
using namespace rdc;
namespace DarkHorse {
	/// <summary>
	/// ���� ���� Ŭ����
	/// </summary>
	class VtFileEventMonitor {
		/// <summary>
		/// ������ ����ȭ Ŭ����
		/// </summary>
		class CriticalSection {
			CRITICAL_SECTION m_cs;
			CriticalSection(const CriticalSection&);
			CriticalSection& operator=(const CriticalSection&);
		public:
			CriticalSection() { ::InitializeCriticalSection(&m_cs); }
			~CriticalSection() { ::DeleteCriticalSection(&m_cs); }
			void Lock() { ::EnterCriticalSection(&m_cs); }
			void Unlock() { ::LeaveCriticalSection(&m_cs); }
		};
		class AutoLock {
			CriticalSection& m_cs;
		public:
			AutoLock(CriticalSection& cs) : m_cs(cs) { m_cs.Lock(); }
			~AutoLock() { m_cs.Unlock(); }
		};

		/// <summary>
		/// �ִ� ���� �ڵ� ����
		/// </summary>
		static const UINT MAX_WAIT_COUNT = 64;

	public:
		VtFileEventMonitor()
			: m_sync()
			, m_shutdownevent(::CreateEvent(NULL, TRUE, FALSE, NULL))
			, m_htWorker(NULL)
			, m_uWorkerThreadId(0)
		{
			// ���� ����� ��ü ����
			changes = new CReadDirectoryChanges(eIoModel, dwMaxChanges);
		}
		virtual ~VtFileEventMonitor()
		{
			// ���� ����� ��ü ������ ����
			if (changes) {
				changes->Terminate();
				delete changes;
				changes = nullptr;
			}

			// ������ ����
			Stop();
		}

		/// <summary>
		/// �۾� ������ ���� - �̺�Ʈ�� �� �� ���� ���ϵȴ�.
		/// </summary>
		/// <returns></returns>
		bool Start()
		{
			// �۾� ������ ����
			m_htWorker = reinterpret_cast<HANDLE>(::_beginthreadex(NULL,
				0,
				VtFileEventMonitor::_ThreadProc,
				this,
				0,
				&m_uWorkerThreadId));
			if (m_htWorker == NULL)
				return false;
			return true;
		}

		/// <summary>
		/// �۾� �����带 �����ϰ� ��� ���õ� �ڿ��� �����Ѵ�.
		/// </summary>
		void Stop()
		{
			// �۾� �����尡 �ִٸ� ���� �̺�Ʈ�� ������ �� ���Ŀ� �����带 ���� ��Ų��.
			if (m_htWorker != NULL) {
				SetEvent(m_shutdownevent);
				::WaitForSingleObject(m_htWorker, INFINITE);
				::CloseHandle(m_htWorker); m_htWorker = NULL;
			}

			// �˴ٿ� �̺�Ʈ �ڵ� ����
			::CloseHandle(m_shutdownevent); m_shutdownevent = NULL;
		}

		/// <summary>
		/// �۾� �������� �ڵ��� ��ȯ�Ѵ�.
		/// </summary>
		/// <returns></returns>
		HANDLE GetThreadHandle()
		{
			return m_htWorker;
		}

		std::vector<std::string> split(const std::string& s, char delimiter)
		{
			std::vector<std::string> tokens;
			std::string token;
			std::istringstream tokenStream(s);
			while (std::getline(tokenStream, token, delimiter))
			{
				tokens.push_back(token);
			}
			return tokens;
		}

		/// <summary>
		/// ���� ��ȭ �̺�Ʈ�� ó���Ѵ�.
		/// </summary>
		/// <param name="action">�׼��� ���� : ����, ����, ����</param>
		/// <param name="fileName">�׼��� �Ͼ ���� �̸�</param>
		void OnFileChanged(DWORD action, CString fileName)
		{
		}
		/// <summary>
		/// ������ ���丮�� �߰��Ѵ�.
		/// </summary>
		/// <param name="filePath">������ �ִ� ���丮 ���(�����̸� �������� ����)</param>
		/// <param name="includingSubDir">���� ���丮 ���ÿ���. ���̸� ���� ���丮 ����, �����̸� ������ ���丮�� ����</param>
		void AddMonDir(LPCTSTR filePath, bool includingSubDir)
		{
			changes->AddDirectory(filePath, true, dwNotificationFlags, dwBufSize);
		}
	private:
		/// <summary>
		/// �۾� ������ ��ü 
		/// </summary>
		/// <returns>�˴ٿ� �̺�Ʈ�� �Ͼ�� ������ ��ƾ�� ����ȴ�.</returns>
		unsigned int ThreadProc()
		{
			const HANDLE handles[] = { m_shutdownevent, changes->GetWaitHandle() };

			char buf[MAX_PATH] = { 0 };
			while (!bTerminate)
			{
				DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
				switch (rc) {
				case WAIT_OBJECT_0 + 0: // Shut down Event
					bTerminate = true;
					break;
				case WAIT_OBJECT_0 + 1: { // We've received a notification in the queue
					if (changes->CheckOverflow()) {
						std::string msg = _T("Queue overflowed.\n");
					}
					else {
						DWORD dwAction;
						CString strFilename;
						changes->Pop(dwAction, strFilename);
						// ���� ���� �̺�Ʈ�� ó���Ѵ�.
						if (dwAction == FILE_ACTION_MODIFIED) {
							nlohmann::json order_info;
							order_info["command"] = 1;
							order_info["file_name"] = (const char*)(strFilename);
							order_info["tried"] = false;

							mainApp.out_system_manager()->AddSignal(std::move(order_info));

						}
					}
				}
									  break;
				case WAIT_IO_COMPLETION:
					// Nothing to do.
					break;
				}
			}

			return 0;
		}

		/// <summary>
		/// �׼��� ����
		/// </summary>
		/// <param name="dwAction"></param>
		/// <returns></returns>
		LPCTSTR ExplainAction(DWORD dwAction)
		{
			switch (dwAction) {
			case FILE_ACTION_ADDED: // ������ �߰���
				return _T("[Add]");
			case FILE_ACTION_REMOVED: // ������ ������
				return _T("[Delete]");
			case FILE_ACTION_MODIFIED: // ������ ������
				return _T("[Modified]");
			case FILE_ACTION_RENAMED_OLD_NAME: // �����̸��� �����̸����� �����
				return _T("[Renamed From]");
			case FILE_ACTION_RENAMED_NEW_NAME:
				return _T("[Renamed To]"); // ���� �̸��� ���� �����
			default:
				return _T("[BAD DATA]"); // �߸��� ������
			}
		}

		/// <summary>
		/// ���� ������ �Լ� - ��ü ��� �Լ��� �θ��� ���� Ŭ���� ���� ������ �Լ�
		/// </summary>
		/// <param name="p"></param>
		/// <returns></returns>
		static unsigned int __stdcall _ThreadProc(void* p)
		{
			_ASSERTE(p != NULL);
			return reinterpret_cast<VtFileEventMonitor*>(p)->ThreadProc();
		}

	private:
		/// <summary>
		/// �̺�Ʈ ó�� ��
		/// </summary>
		EIoModelType eIoModel = eIoModel_CompletionRoutine;
		/// <summary>
		/// �ִ� ���� �̺�Ʈ ����
		/// </summary>
		DWORD dwMaxChanges = 4096;
		/// <summary>
		/// ���� ũ��
		/// </summary>
		DWORD dwBufSize = 16 * 1024;
		/// <summary>
		/// ���� ���� �÷���
		/// </summary>
		const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE
			| FILE_NOTIFY_CHANGE_CREATION
			| FILE_NOTIFY_CHANGE_FILE_NAME
			| FILE_NOTIFY_CHANGE_DIR_NAME;
		/// <summary>
		/// ���� ���� Ŭ���� ��ü ������
		/// </summary>
		CReadDirectoryChanges* changes = nullptr;

		/// <summary>
		/// ������ ����ȭ ��ü
		/// </summary>
		CriticalSection m_sync;
		/// <summary>
		/// �˴ٿ� �̺�Ʈ �ڵ�
		/// </summary>
		HANDLE m_shutdownevent;
		/// <summary>
		/// �۾� ������ �ڵ�
		/// </summary>
		HANDLE m_htWorker;
		/// <summary>
		/// �۾� ������ ���̵�
		/// </summary>
		unsigned m_uWorkerThreadId;
		/// <summary>
		/// ������ ���� ���� ����
		/// </summary>
		bool bTerminate = false;
	};
}