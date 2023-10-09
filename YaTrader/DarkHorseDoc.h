// DarkHorseDoc.h : interface of the CDarkHorseDoc class
//


#pragma once


class CDarkHorseDoc : public CDocument
{
protected: // create from serialization only
	CDarkHorseDoc();
	DECLARE_DYNCREATE(CDarkHorseDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CDarkHorseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	CView* GetDarkHorseVeiw(void);
};


