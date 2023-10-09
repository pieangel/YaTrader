#pragma once
#include <map>
#include <string>
#include <any>
#include "SmTaskTypeDef.h"

namespace DarkHorse {
	static std::map<SmTaskType, std::string> SmTaskTitleMap = {
		{SmTaskType::MasterFileDownload, "마스터파일을 가져오고 있습니다."},
		{SmTaskType::DomesticSymbolCode, "국내종목코드를 가져오고 있습니다."},
		{SmTaskType::FileDownload, "파일을 다운로드 하고 있습니다."},
		{SmTaskType::Market, "시장 정보를 가져오고 있습니다."},
		{SmTaskType::SymbolCode, "종목 코드를 가져오고 있습니다."},
		{SmTaskType::SymbolMaster, "종목 정보를 가져오고 있습니다."},
		{SmTaskType::AccountAsset, "자산 정보를 가져오고 있습니다."},
		{SmTaskType::AccountProfitLoss, "계좌별 손익을 가져오고 있습니다."},
		{SmTaskType::SymbolProfitLoss, "종목별 손익을 가져오고 있습니다."},
		{SmTaskType::SymbolQuote, "시세를 가져오고 있습니다."},
		{SmTaskType::SymbolHoga, "호가를 가져오고 있습니다."},
		{SmTaskType::ChartData, "차트데이터를 가져오고 있습니다."},
		{SmTaskType::AcceptedOrderList, "접수주문목록을 가져오고 있습니다."},
		{SmTaskType::FilledOrderList, "체결주문목록을 가져오고 있습니다."},
		{SmTaskType::SymbolPosition, "종목별 포지션을 가져오고 있습니다."},
		{SmTaskType::RegisterSymbol, "종목을 등록하고 있습니다."},
		{SmTaskType::RegisterAccount, "계좌를 등록하고 있습니다."},
	};
}