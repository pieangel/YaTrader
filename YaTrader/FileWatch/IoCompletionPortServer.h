#pragma once

#include "ReadDirectoryServer.h"
#include "IoCompletionPort.h"
namespace rdc
{
// IO Completion Port 서버 - IO 완료 감시 서버
class CIoCompletionPortServer :
    public CReadDirectoryServer
{
    struct RemoveDirectoryIocpArg;
public:
    CIoCompletionPortServer(CReadDirectoryChanges* pHost);
    virtual ~CIoCompletionPortServer(void);

    virtual void Stop(void) override;
    virtual bool AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
        DWORD dwNotifyFilter, DWORD dwBufferSize) override;
    virtual bool RemoveDirectory(const CString& strDirectory) override;
protected:
    virtual void Run() override;
private:
    // running in work server thread
    void ProcessIocpSuccess(ULONG_PTR pCompKey, DWORD dwNumberOfBytes, OVERLAPPED* pOverlapped);
    bool ProcessIocpError(DWORD dwLastError, OVERLAPPED* pOverlapped);
    void ProcessKeyAddDirectory(CReadDirectoryRequest* pRequest);
    void ProcessKeyReadDirectory(CReadDirectoryRequest* pRequest, DWORD dwNumberOfBytes);
    void ProcessKeyRemoveDirectory(RemoveDirectoryIocpArg* pArg);
    void ProcessKeyTerminite(void);
private:
    struct RemoveDirectoryIocpArg :
        public OVERLAPPED
    {
        CIoCompletionPortServer* pServer;
        CString strDirectory;
        RemoveDirectoryIocpArg(CIoCompletionPortServer* server, const CString& directory) :
            pServer(server), strDirectory(directory) 
        {
            ZeroMemory(this, sizeof(OVERLAPPED));
        }
    };

    CIOCP m_iocp;

    const ULONG_PTR KEY_ADD_DIRECTORY = 1;
    const ULONG_PTR KEY_READ_DIRECTORY = 2;
    const ULONG_PTR KEY_REMOVE_DIRECTORY = 3;
    const ULONG_PTR KEY_TERMINITE = 4;
};

}