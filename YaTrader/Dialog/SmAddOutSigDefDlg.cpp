// VtAddOutSigDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAddOutSigDefDlg.h"
#include "afxdialogex.h"

#include <memory>
#include <algorithm>


// VtAddOutSigDefDlg dialog

IMPLEMENT_DYNAMIC(SmAddOutSigDefDlg, CBCGPDialog)

SmAddOutSigDefDlg::SmAddOutSigDefDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_ADD_SIG_DEF, pParent)
{

}

SmAddOutSigDefDlg::~SmAddOutSigDefDlg()
{
}

void SmAddOutSigDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SIGNAL, _EditSignal);
}


BEGIN_MESSAGE_MAP(SmAddOutSigDefDlg, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &SmAddOutSigDefDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// VtAddOutSigDefDlg message handlers


void SmAddOutSigDefDlg::OnBnClickedOk()
{
// 	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
// 	SharedOutSigDef sig = std::make_shared<VtOutSignalDef>();
// 	CString strDesc, strSymbol, strSignal, strStrategy;
// 	_EditSignal.GetWindowText(strSignal);
// 	sig->SignalName = (LPCTSTR)strSignal;
// 	sig->SymbolCode = (LPCTSTR)strSymbol;
// 	sig->StrategyName = (LPCTSTR)strStrategy;
// 	sig->Desc = (LPCTSTR)strDesc;
// 	outSigDefMgr->AddOutSignalDef(sig);
// 	if (_SigDefGrid) _SigDefGrid->AddOutSigDef(sig);
	CBCGPDialog::OnOK();
}
