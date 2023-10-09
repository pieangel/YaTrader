// DataProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "DataProgressDialog.h"


// DataProgressDialog dialog

IMPLEMENT_DYNAMIC(DataProgressDialog, CBCGPDialog)

DataProgressDialog::DataProgressDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DATA_PROGRESS, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

DataProgressDialog::~DataProgressDialog()
{
}

void DataProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_DATA, _ProgressData);
	DDX_Control(pDX, IDC_STATIC_DETAIL, _StaticDetail);
}


void DataProgressDialog::SetTotalCount(const int& count)
{
	_TotalCount = count;
}

void DataProgressDialog::SetRemainCount(const int& count)
{
	_RemainCount = count;
}

void DataProgressDialog::SetTaskTitle(const std::string& title)
{
	SetWindowText(title.c_str());
}

void DataProgressDialog::SetTaskDetail(const std::string& detail)
{
	_StaticDetail.SetWindowText(detail.c_str());
}

void DataProgressDialog::RefreshProgress()
{
	double top = (double)_TotalCount - (double)_RemainCount;
	double entire = top / (double)_TotalCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
	_StaticDetail.Invalidate(FALSE);
}

void DataProgressDialog::ShowHide(const bool show)
{
	show ? ShowWindow(SW_SHOW) : ShowWindow(SW_HIDE);
}

BEGIN_MESSAGE_MAP(DataProgressDialog, CBCGPDialog)
END_MESSAGE_MAP()


// DataProgressDialog message handlers


BOOL DataProgressDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool DataProgressDialog::GetShowState() const
{
	if (IsWindowVisible() == TRUE) return true;
	else return false;

}

void DataProgressDialog::SetPrgressPos(const int& pos)
{
	_ProgressData.SetPos(pos);
}
