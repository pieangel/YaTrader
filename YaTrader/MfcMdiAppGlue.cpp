/**
 * author: Mag. Christian Kleinheinz <BR>
 * Copyright 2008, MasterSoft Software Solutions Ltd. 
 * Dev. BGH (Mag. Christian Kleinheinz)
 *
 * The code is released under the Code Project Open License (CPOL) 
 * Additional information can be found at our hompage http://www.mastersoft.at.
 */
#include "StdAfx.h"
#include ".\mfcmdiappglue.h"

CDocument* ACMfcMdiAppGlue::GetActiveDoc()
{
	CWinApp* pApp = AfxGetApp();
	
	if((!pApp) || (!pApp->m_pMainWnd))
		return(NULL);

    CMDIChildWnd * pChild = ((CMDIFrameWnd*)(pApp->m_pMainWnd))->MDIGetActive();

    if (!pChild)
        return(NULL);

    CDocument * pDoc = pChild->GetActiveDocument();

    if (!pDoc)
        return(NULL);

    return (pDoc);
}

CView* ACMfcMdiAppGlue::GetActiveViewWnd()
{
	CWinApp* pApp = AfxGetApp();
	
	if((!pApp) || (!pApp->m_pMainWnd))
		return(NULL);

    CMDIChildWnd * pChild = ((CMDIFrameWnd*)(pApp->m_pMainWnd))->MDIGetActive();

    if ( !pChild )
        return NULL;

	CView* pView = pChild->GetActiveView();

	if(!pView)
		return(NULL);

	return(pView);
}

BOOL ACMfcMdiAppGlue::CreateNewDocumentInstanceTriple(CDocument*& pDoc)
{
	CWinApp *pApp = AfxGetApp();
	CDocManager *pDocMgr = pApp->m_pDocManager;
	CView* pView = NULL;

	for(POSITION TmplPos = pDocMgr->GetFirstDocTemplatePosition();TmplPos;)
	{
		//create the document/feame/view trible..
		CMultiDocTemplate *pTmpl = (CMultiDocTemplate *)pDocMgr->GetNextDocTemplate(TmplPos);
		ASSERT_VALID(pTmpl);		
		pDoc = pTmpl->OpenDocumentFile(NULL,TRUE);
		break;
	}

	if(pDoc==NULL)
		return(FALSE);
	return(TRUE);
}

void ACMfcMdiAppGlue::GetAllDocuments(t_DocumentList &Docs)
{
	CWinApp *pApp = AfxGetApp();
	CDocManager *pDocMgr = pApp->m_pDocManager;  
	// For all document templates
	for(POSITION TmplPos = pDocMgr->GetFirstDocTemplatePosition();
		TmplPos;)
	{
		CDocTemplate *pTmpl = pDocMgr->GetNextDocTemplate(TmplPos);
		
		ASSERT_VALID(pTmpl);  
		if(pTmpl == NULL)
			continue;

		// For All open documents of this document template type.
		for(POSITION Pos = pTmpl->GetFirstDocPosition();Pos;) 
		{
			CDocument *pDoc = pTmpl->GetNextDoc(Pos);	  	  
			Docs.AddTail(pDoc);
		}
	}
}

void ACMfcMdiAppGlue::GetAllViews(t_ViewList &Views)
{
	CWinApp *pApp = AfxGetApp();
	CDocManager *pDocMgr = pApp->m_pDocManager;
	// For all document templates
	for(POSITION TmplPos = pDocMgr->GetFirstDocTemplatePosition();
		TmplPos;)
	{
		CDocTemplate *pTmpl = pDocMgr->GetNextDocTemplate(TmplPos);
		
		ASSERT_VALID(pTmpl);
		if(pTmpl == NULL)
			continue;

		// For All open documents of this document template type.
		for(POSITION Pos = pTmpl->GetFirstDocPosition();Pos;) 
		{
			CDocument *pDoc = pTmpl->GetNextDoc(Pos);	  	  
			// For all views of this document
			POSITION ViewPos = pDoc->GetFirstViewPosition();
			while(ViewPos)
			{
				CView *pView =  pDoc->GetNextView(ViewPos); 		
				Views.AddTail(pView);
			}
		}
	}
}

CView* ACMfcMdiAppGlue::GetFirstView()
{
	CWinApp* pApp = AfxGetApp();
	CDocManager* pDocMgr = pApp->m_pDocManager;
	// For all document templates
	for (POSITION TmplPos = pDocMgr->GetFirstDocTemplatePosition();
		TmplPos;)
	{
		CDocTemplate* pTmpl = pDocMgr->GetNextDocTemplate(TmplPos);

		ASSERT_VALID(pTmpl);
		if (pTmpl == NULL)
			continue;

		// For All open documents of this document template type.
		for (POSITION Pos = pTmpl->GetFirstDocPosition(); Pos;)
		{
			CDocument* pDoc = pTmpl->GetNextDoc(Pos);
			// For all views of this document
			POSITION ViewPos = pDoc->GetFirstViewPosition();
			while (ViewPos)
			{
				CView* pView = pDoc->GetNextView(ViewPos);
				return pView;
			}
		}
	}

	return NULL;
}

void ACMfcMdiAppGlue::GetAllFrameWnds(t_FrameList &Frames)
{
	CWinApp *pApp = AfxGetApp();
	CDocManager *pDocMgr = pApp->m_pDocManager;  
	// For all document templates
	for(POSITION TmplPos = pDocMgr->GetFirstDocTemplatePosition();
		TmplPos;)
	{
		CDocTemplate *pTmpl = pDocMgr->GetNextDocTemplate(TmplPos);
		ASSERT_VALID(pTmpl);
		if(pTmpl == NULL)
			continue;

		// For All open documents of this document template type.
		for(POSITION Pos = pTmpl->GetFirstDocPosition();Pos;) 
		{
			CDocument *pDoc = pTmpl->GetNextDoc(Pos);	  	  
			// get me a view and from it the MDI child frame
			POSITION ViewPos = pDoc->GetFirstViewPosition();
			CView *pView =  pDoc->GetNextView(ViewPos); 	
			CFrameWnd *pFrame = pView->GetParentFrame();			
			ASSERT_VALID(pFrame);
			Frames.AddTail(pFrame);    
		}
	}
}

