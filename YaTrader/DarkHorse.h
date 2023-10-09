// DarkHorse.h : main header file for the DarkHorse application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CDarkHorseApp:
// See DarkHorse.cpp for the implementation of this class
//

class CDarkHorseApp : public CBCGPWinApp
{
public:
	CDarkHorseApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL SaveAllModified();


// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	void InitLog();
	int RegisterOCX();
	void CreateCrashHandler();
	bool CheckExpire();
};

extern CDarkHorseApp theApp;
