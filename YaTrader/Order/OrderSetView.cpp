#include "stdafx.h"
#include "OrderSetView.h"
#include "../Grid/CustomCells.h"

IMPLEMENT_DYNAMIC(OrderSetView, CBCGPGridCtrl)

OrderSetView::OrderSetView()
{
	m_bExtendedPadding = FALSE;
}

OrderSetView::~OrderSetView()
{
}

void OrderSetView::OnHeaderCheckBoxClick(int nColumn)
{
	CString msg;
	msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	if (_HeaderCheck) {
		_HeaderCheck = false;

		CheckAll(FALSE);

	}
	else {
		_HeaderCheck = true;

		CheckAll(TRUE);
	}
	UpdateHeaderCheckbox();
	Invalidate();
}

void OrderSetView::OnRowCheckBoxClick(CBCGPGridRow* pRow)
{
	CString msg;
	msg.Format("%d", pRow->GetRowId());
	//AfxMessageBox(msg);
	if (pRow->GetCheck() == 1)
		pRow->SetCheck(0);
	else if (pRow->GetCheck() == 0)
		pRow->SetCheck(1);
	Invalidate();
}

BEGIN_MESSAGE_MAP(OrderSetView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int OrderSetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableMarkSortedColumn(FALSE);
	EnableHeader(FALSE, ~BCGP_GRID_HEADER_SORT);
	EnableRowHeader(TRUE);
	SetClearInplaceEditOnEnter(FALSE);
	EnableInvertSelOnCtrl();
	SetWholeRowSel(FALSE);

	SetRowHeaderWidth(1);
	//SetGridHeaderHeight(1);
	
	SetVisualManagerColorTheme(TRUE);

	EnableAlternateRows(FALSE);
	
	
	InsertColumn(0, _T("A"), globalUtils.ScaleByDPI(75));
	InsertColumn(1, _T("B"), globalUtils.ScaleByDPI(40));
	InsertColumn(2, _T("C"), globalUtils.ScaleByDPI(75));
	InsertColumn(3, _T("D"), globalUtils.ScaleByDPI(40));
	InsertColumn(4, _T("E"), globalUtils.ScaleByDPI(80));
	InsertColumn(5, _T("F"), globalUtils.ScaleByDPI(80));
	InsertColumn(6, _T("F"), globalUtils.ScaleByDPI(60));



	for (int i = 0; i < GetColumnCount(); i++)
	{
		
		SetColumnAlign(i, 2);

	}
	CBCGPGridRow* pRow = nullptr;
	CBCGPGridItem* pSpinItem = nullptr;
	CButtonItem* pButtonItem = nullptr;
	/*
	pRow = CreateRow(GetColumnCount());
	for (int i = 0; i < GetColumnCount(); i++)
	{
		pRow->GetItem(i)->AllowEdit(FALSE);
		SetColumnAlign(0, 2);
	
	}

	
	

	CBCGPGridItem* pSpinItem = new CBCGPGridItem(1);
	pSpinItem->EnableSpinControl(TRUE, 0, 100);

	pRow->GetItem(0)->SetValue("수량");
	pRow->GetItem(0)->GetAlign();

	pRow->ReplaceItem(1, pSpinItem);

	CButtonItem* pButtonItem = new CButtonItem("1", 1);
	pRow->ReplaceItem(2, pButtonItem);

	pButtonItem = new CButtonItem("2", 2);
	pRow->ReplaceItem(3, pButtonItem);

	pButtonItem = new CButtonItem("3", 3);
	pRow->ReplaceItem(4, pButtonItem);

	pButtonItem = new CButtonItem("4", 4);
	pRow->ReplaceItem(5, pButtonItem);

	pButtonItem = new CButtonItem("5", 5);
	pRow->ReplaceItem(6, pButtonItem);

	AddRow(pRow, FALSE);
	*/

	SetCustomRowHeight(22);

	pRow = CreateRow(GetColumnCount());

	
	CBCGPGridCheckItem* pCheckItem = new CBCGPGridCheckItem(TRUE);
	pCheckItem->SetLabel(_T("이익실현"));
	
	pRow->ReplaceItem(0, pCheckItem);

	pSpinItem = new CBCGPGridItem(2);
	pSpinItem->EnableSpinControl(TRUE, 0, 1000);

	pRow->ReplaceItem(1, pSpinItem);

	pCheckItem = new CBCGPGridCheckItem(TRUE);
	pCheckItem->SetLabel(_T("손실제한"));
	
	pRow->ReplaceItem(2, pCheckItem);

	pSpinItem = new CBCGPGridItem(2);
	pSpinItem->EnableSpinControl(TRUE, 0, 1000);
	pRow->ReplaceItem(3, pSpinItem);
	/*
// 	CBCGPGridCheckItem* pSwitchItem = new CBCGPGridCheckItem(FALSE);
// 	pSwitchItem->SetMode(CBCGPGridCheckItem::Switch);
// 	pSwitchItem->SetLabel(_T("시장가"));
// 	pRow->ReplaceItem(4, pSwitchItem);

	CRadioButtonItem* pRadio = new CRadioButtonItem("시장가", 6);
	pRadio->Active(true);
	pRow->ReplaceItem(4, pRadio);

	//CBCGPGridCheckItem* pSwitchItem = new CBCGPGridCheckItem(FALSE);
	//pSwitchItem->SetMode(CBCGPGridCheckItem::Switch);
	//pSwitchItem->SetLabel(_T("지정가"));
	//pRow->ReplaceItem(5, pSwitchItem);

	pRadio = new CRadioButtonItem("지정가", 7);
	pRow->ReplaceItem(5, pRadio);
	*/

	CBCGPGridItem* pItem = new CBCGPGridItem("시장가");
	pItem->SetReadOnly(TRUE);
	pItem->SetValue(_T("시장가"));

	pItem->AddOption("시장가", 1);
	pItem->AddOption("지정가", 1);

	pRow->ReplaceItem(4, pItem);

	pRow->GetItem(5)->SetValue("STOP");

	pSpinItem = new CBCGPGridItem(2);
	pSpinItem->EnableSpinControl(TRUE, 0, 1000);
	pRow->ReplaceItem(6, pSpinItem);

	AddRow(pRow, FALSE);


	pRow = CreateRow(GetColumnCount());

	pRow->GetItem(0)->SetValue("매수가능");
	pSpinItem = new CBCGPGridItem(2);
	pSpinItem->EnableSpinControl(FALSE, 0, 1000);
	pRow->ReplaceItem(1, pSpinItem);

	pRow->GetItem(2)->SetValue("매도가능");
	pSpinItem = new CBCGPGridItem(2);
	pSpinItem->EnableSpinControl(FALSE, 0, 1000);
	pRow->ReplaceItem(3, pSpinItem);

	pCheckItem = new CBCGPGridCheckItem(TRUE);
	pCheckItem->SetLabel(_T("호가고정"));

	pRow->ReplaceItem(4, pCheckItem);

	pButtonItem = new CButtonItem("모두청산", 6);
	pRow->ReplaceItem(5, pButtonItem);

	pCheckItem = new CBCGPGridCheckItem(TRUE);
	pCheckItem->SetLabel(_T("체결틱"));

	pRow->ReplaceItem(6, pCheckItem);

	AddRow(pRow, FALSE);

	return 0;
}

void OrderSetView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void OrderSetView::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}


void OrderSetView::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);

	if (id.m_nRow == 0) {
		CBCGPGridRow* pRow = GetRow(id.m_nRow);
		if (id.m_nColumn == 0) {
			if (((CBCGPGridCheckItem*)pRow->GetItem(0))->GetState() == 1)
				((CBCGPGridCheckItem*)pRow->GetItem(0))->SetState(0);
			else
				((CBCGPGridCheckItem*)pRow->GetItem(0))->SetState(1);
		}
		else if (id.m_nColumn == 4) {
			if (((CRadioButtonItem*)pRow->GetItem(4))->Active()) {
				((CRadioButtonItem*)pRow->GetItem(4))->Active(false);
				((CRadioButtonItem*)pRow->GetItem(5))->Active(true);
			}
			else {
				((CRadioButtonItem*)pRow->GetItem(4))->Active(true);
				((CRadioButtonItem*)pRow->GetItem(5))->Active(false);
			}
		}
		else if (id.m_nColumn == 5) {
			if (((CRadioButtonItem*)pRow->GetItem(5))->Active()) {
				((CRadioButtonItem*)pRow->GetItem(5))->Active(false);
				((CRadioButtonItem*)pRow->GetItem(4))->Active(true);
			}
			else {
				((CRadioButtonItem*)pRow->GetItem(5))->Active(true);
				((CRadioButtonItem*)pRow->GetItem(4))->Active(false);
			}
		}
		else if (id.m_nColumn == 1) {
			CBCGPGridCtrl::OnLButtonDown(nFlags, point);
		}
		else {
			CBCGPGridCtrl::OnLButtonDown(nFlags, point);
		}
	}
	else {
		CBCGPGridCtrl::OnLButtonDown(nFlags, point);
	}
	//Invalidate();
	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}
