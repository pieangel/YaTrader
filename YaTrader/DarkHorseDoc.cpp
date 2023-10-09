// DarkHorseDoc.cpp : implementation of the CDarkHorseDoc class
//

#include "stdafx.h"
#include "DarkHorse.h"

#include "DarkHorseDoc.h"
#include "DarkHorseView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDarkHorseDoc

IMPLEMENT_DYNCREATE(CDarkHorseDoc, CDocument)

BEGIN_MESSAGE_MAP(CDarkHorseDoc, CDocument)
END_MESSAGE_MAP()


// CDarkHorseDoc construction/destruction

CDarkHorseDoc::CDarkHorseDoc()
{
	// TODO: add one-time construction code here

}

CDarkHorseDoc::~CDarkHorseDoc()
{
}

BOOL CDarkHorseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CDarkHorseDoc serialization

void CDarkHorseDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CDarkHorseDoc diagnostics

#ifdef _DEBUG
void CDarkHorseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDarkHorseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

CView* CDarkHorseDoc::GetDarkHorseVeiw(void)
{
	CRuntimeClass* prt = RUNTIME_CLASS(CDarkHorseView);
	CView* pView = NULL;

	// Continue search in inactive View by T(o)m

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		if (pView->GetRuntimeClass() == prt)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CDarkHorseView)))
				break;
		}
		pView = NULL;       // not valid vie
	}

	return static_cast<CDarkHorseView*>(pView);
}

#endif //_DEBUG


// CDarkHorseDoc commands
