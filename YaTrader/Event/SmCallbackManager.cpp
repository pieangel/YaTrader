#include "stdafx.h"
#include "SmCallbackManager.h"
#include "../Symbol/SmSymbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DarkHorse;

SmCallbackManager::SmCallbackManager()
{
}


SmCallbackManager::~SmCallbackManager()
{
	ClearAllEventMap();
}

void SmCallbackManager::UnsubscribeQuoteCallback(long id)
{
	auto it = _QuoteMap.find(id);
	if (it != _QuoteMap.end()) {
		_QuoteMap.erase(it);
	}
}

void SmCallbackManager::OnQuoteEvent(const std::string& symbol_code)
{

	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _QuoteMap.begin(); it != _QuoteMap.end(); ++it) {
		it->second(symbol_code);
	}
}

void SmCallbackManager::OnHogaEvent(const std::string& symbol_code)
{

	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _HogaMap.begin(); it != _HogaMap.end(); ++it) {
		it->second(symbol_code);
	}
}

void SmCallbackManager::UnsubscribeHogaCallback(long id)
{
	auto it = _HogaMap.find(id);
	if (it != _HogaMap.end()) {
		_HogaMap.erase(it);
	}
}

void SmCallbackManager::UnsubscribeOrderCallback(long id)
{
	auto it = _OrderMap.find(id);
	if (it != _OrderMap.end()) {
		_OrderMap.erase(it);
	}
}

void SmCallbackManager::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{

	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		it->second(account_no, symbol_code);
	}
}

void SmCallbackManager::UnsubscribeMasterCallback(long id)
{
	auto it = _MasterMap.find(id);
	if (it != _MasterMap.end()) {
		_MasterMap.erase(it);
	}
}

void SmCallbackManager::OnMasterEvent(const std::string& symbol_code)
{
	
	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _MasterMap.begin(); it != _MasterMap.end(); ++it) {
		it->second(symbol_code);
	}
}

void SmCallbackManager::UnsubscribeChartCallback(long id)
{
	auto it = _ChartMap.find(id);
	if (it != _ChartMap.end()) {
		_ChartMap.erase(it);
	}
}



void SmCallbackManager::UnsubscribeAccountCallback(long id)
{
	auto it = _AccountMap.find(id);
	if (it != _AccountMap.end()) {
		_AccountMap.erase(it);
	}
}

void SmCallbackManager::OnAccountEvent(const int& account_id)
{

	for (auto it = _AccountMap.begin(); it != _AccountMap.end(); ++it) {
		it->second(account_id);
	}
}



void SmCallbackManager::OnWndQuoteEvent(const int& symbol_id)
{

	for (auto it = _WndQuoteSet.begin(); it != _WndQuoteSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_QUOTE_RECEIVED, (WPARAM)symbol_id, 0);
	}
}



void SmCallbackManager::OnWndHogaEvent(const int& symbol_id)
{
	
	for (auto it = _WndHogaSet.begin(); it != _WndHogaSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_HOGA_RECEIVED, (WPARAM)symbol_id, 0);
	}
}

void SmCallbackManager::OnWndOrderEvent(const std::string& order_no)
{
	// WM_ORDER_CHANGED
	
}

void SmCallbackManager::OnWndAccountEvent(const int& account_id)
{
	for (auto it = _WndAccountSet.begin(); it != _WndAccountSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_ACCOUNT_CHANGED, (WPARAM)account_id, 0);
	}
}


void SmCallbackManager::OnWndChartEvent(UINT Msg, SmChartData* chartData)
{
	for (auto it = _WndChartSet.begin(); it != _WndChartSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, Msg, 0, (LPARAM)chartData);
	}
}

void SmCallbackManager::OnSymbolListReceived()
{
	for (auto it = _WndChartSet.begin(); it != _WndChartSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_SYMBOL_LIST_RECEIVED, 0, 0);
	}
}

void SmCallbackManager::ClearAllEventMap()
{
	_WndQuoteSet.clear();
	_WndHogaSet.clear();
	_WndOrderSet.clear();
	_WndAccountSet.clear();
}

void DarkHorse::SmCallbackManager::OnSymbolMasterEvent(const int& symbol_id)
{
	for (auto it = _WndSymbolMasterSet.begin(); it != _WndSymbolMasterSet.end(); it++) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_SYMBOL_MASTER_RECEIVED, (WPARAM)symbol_id, 0);
	}
}

void DarkHorse::SmCallbackManager::OnWndOrderEvent(const int& account_id, const int& symbol_id)
{
	for (auto it = _WndOrderSet.begin(); it != _WndOrderSet.end(); it++) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_ORDER_CHANGED, (WPARAM)account_id, (LPARAM)symbol_id);
	}
}

void DarkHorse::SmCallbackManager::OnOrderUpdateEvent(const int& account_id, const int& symbol_id)
{
	for (auto it = _OrderUpdateSet.begin(); it != _OrderUpdateSet.end(); it++) {
		HWND hWnd = *it;
		::PostMessage(hWnd, WM_ORDER_UPDATE, (WPARAM)account_id, (LPARAM)symbol_id);
	}
}

void DarkHorse::SmCallbackManager::OnPasswordConfirmed(const int& account_id, const int& result)
{
	for (auto it = _WndPasswordSet.begin(); it != _WndPasswordSet.end(); it++) {
		HWND hWnd = *it;
		::PostMessage(hWnd, WM_PASSWORD_CONFIRMED, (WPARAM)account_id, (LPARAM)result);
	}
}

void DarkHorse::SmCallbackManager::OnServerMsg(const int& msg_id)
{
	for (auto it = _WndServerMsgSet.begin(); it != _WndServerMsgSet.end(); it++) {
		HWND hWnd = *it;
		::PostMessage(hWnd, WM_SERVER_MSG, (WPARAM)msg_id, (LPARAM)0);
	}
}

void DarkHorse::SmCallbackManager::OnFundChanged()
{
	for (auto it = _FundWndSet.begin(); it != _FundWndSet.end(); it++) {
		HWND hWnd = *it;
		::PostMessage(hWnd, WM_FUND_CHANGED, (WPARAM)0, (LPARAM)0);
	}
}

void DarkHorse::SmCallbackManager::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
{
	if (!chart_data)
		return;
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		it->second(chart_data, action);
	}
}

