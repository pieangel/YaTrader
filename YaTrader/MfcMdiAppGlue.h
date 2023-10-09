/**
 * author: Mag. Christian Kleinheinz <BR>
 * Copyright 2008, MasterSoft Software Solutions Ltd. 
 * Dev. BGH (Mag. Christian Kleinheinz)
 *
 * The code is released under the Code Project Open License (CPOL) 
 * Additional information can be found at our hompage http://www.mastersoft.at.
 */
#pragma once

#include "stdafx.h"

typedef CTypedPtrList<CPtrList,CView*>     t_ViewList;
typedef CTypedPtrList<CPtrList,CDocument*> t_DocumentList;
typedef CTypedPtrList<CPtrList,CFrameWnd*> t_FrameList;

class ACMfcMdiAppGlue
{
public:
	virtual ~ACMfcMdiAppGlue();

	//Get the current active view window
	static CView* GetActiveViewWnd();
	
	//Get the current active document
	static CDocument* GetActiveDoc();
	
	//create a new instance of the document with a doc/view/frame triple 
	//return a doc pntr
	static BOOL CreateNewDocumentInstanceTriple(CDocument*& pDoc);

	// Function fills a list of all document objects in the application
	static void GetAllDocuments(t_DocumentList &Docs);

	// Function fills a list of all views in the application
	static void GetAllViews(t_ViewList &Views);

	static CView* GetFirstView();

	// Function fills a list of all Child frames in the application
	// i.e. all CFrameWnd's except that returned by AfxGetMainWnd().
	static void GetAllFrameWnds(t_FrameList &Frames);
};
