#pragma once

#include "ReadDirectoryServer.h"

namespace rdc
{

class CCompletionRoutineServer :
    public CReadDirectoryServer
{
public:
    CCompletionRoutineServer(CReadDirectoryChanges* pHost);
    virtual ~CCompletionRoutineServer(void);

    virtual void Stop(void) override;
    virtual bool AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
        DWORD dwNotifyFilter, DWORD dwBufferSize) override;
    virtual bool RemoveDirectory(const CString& strDirectory) override;
protected:
    virtual void Run() override;
private:
    static void CALLBACK AddDirectoryProc(ULONG_PTR pArg);
    static void CALLBACK RemoveDirectoryProc(ULONG_PTR pArg);
    static void CALLBACK TerminateProc(ULONG_PTR pArg);
private:
    struct RemoveDirectoryApcArg
    {
        CCompletionRoutineServer* pServer;
        CString strDirectory;
        RemoveDirectoryApcArg(CCompletionRoutineServer* server, const CString& directory) :
            pServer(server), strDirectory(directory) 
        {}
    };

    bool m_bTerminate;
};

}