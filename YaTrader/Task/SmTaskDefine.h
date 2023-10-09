#pragma once
#include<string>
#include <map>
#include "../Json/json.hpp"

enum class DhTaskType
{
	// 초기 상태
	None = 0,
	// 계좌 손익
	AccountProfitLoss,
	// Chart Data
	SymbolChartData,
	RegisterSymbol,
	// 지수 선물 심볼 
	DmJisuFutureSymbol,
	// 지수 옵션 심볼 
	DmJisuOptionSymbol,
	// 상품 선물 심볼 
	DmCommodityFutureSymbol,
	// 미니 코스피 선물 심볼 
	DmMiniFutureSymbol,
	// 미니 코스피 선물 옵션 심볼 
	DmMiniOptionSymbol,
	// EFT 심볼 받음
	DmEtfSymbol,
	/// <summary>
	/// 위클리 옵션 심볼 받음
	/// </summary>
	DmWeeklyOpSymbol,
	// 해외 선물 심볼 목록 받음
	AbroadSymbol,
	AbSymbolCode, // 심볼 코드
	ABProductRemain, // 종목별 잔고와 평가 손익
	AbAccountOrder, // 계좌별 주문 내역
	AbSymbolMaster, // 해외 종목 정보
	AbSymbolQuote, // 해외 종목 시세
	AbSymbolHoga, // 해외 종목 호가
	AbSymbolChartData, // 해외 종목 차트 데이터
	AbAccountAsset, // 해외 계좌 
	AbAccountProfitLoss, // 해외 계좌 손익
	AbSymbolProfitLoss, // 해외 종목별 손익
	AbSymbolPosition, // 해외 종목 잔고
	AbAcceptedOrderList, // 해외 미체결 주문
	AbFilledOrderList, // 해외 체결 주문
	AbFilledDetail, // 해외 체결 주문
	AbSymbolMasterFileDownload, // 심볼 마스터 파일 다운로드

	DmAcceptedOrderList, // 접수확인 내역
	DmFilledHistory, // 체결내역
	DmSymbolPosition, // 잔고내역
	DmSymbolProfitLoss, // 국내 종목별 손익
	DmOutstanding, // 현재잔고
	DmCmeAcceptedHistory, // cme 접수확인 내역
	DmCmeFilledHistory, // cme 체결내역
	DmCmeOutstandingHistory, // cme 잔고내역
	DmCmeOutstanding, // cme 잔고
	DmCmeAsset, // cme 자산
	DmCmePureAsset, // cme 순자산
	DmAccountAsset, // 자산
	DmDeposit, // 계좌 정보
	DmDailyProfitLoss, // 일일 손익
	DmFilledHistoryTable, // 체결 내역
	DmAccountProfitLoss, // 계좌별 손익
	DmSymbolCode, // 심볼 코드
	DmTradableCodeTable, // 거래가능 종목표
	DmApiCustomerProfitLoss, // 고객 손익 현황
	DmChartData, // 차트 데이터
	DmCurrentQuote, // 현재 시세
	DmDailyQuote, // 일별 시세
	DmTickQuote, // 틱 시세
	DmSecondQutoe, // 장외 시세
	DmSymbolMaster, // 심볼 마스터
	DmStockFutureSymbolMaster, // 증권선물 심볼 마스터
	DmIndustryMaster, // 산업별 마스터
	DmTaskComplete, // 작업 완료
	DmRecentMonthSymbolMaser, // 최근월 심볼 마스터
	DmOrderNew, // 신규주문
	DmOrderChange, // 정정 주문
	DmOrderCancel, // 취소 주문
	DmAccountFeeInfoStep1, // 계좌 정보
	DmAccountFeeInfoStep2, // 계좌 정보
	DmSymbolFileDownload, // 심볼파일 다운로드
	DmSymbolMasterFileDownload // 심볼 마스터 파일 다운로드
};

struct DhTaskArg {
	// 요청 아이디
	int argument_id = 0;
	// 요청했는지 여부
	bool requested = false;
	// 받았는지 여부
	bool received = false;
	DhTaskType task_type = DhTaskType::None;
	// 작업 설명
	std::string description;
	// 작업 내용
	std::string detail_task_description;
	// 매개변수 맵
	nlohmann::json parameter_map;
};

struct DhTaskInfo {
	DhTaskType task_type;
	std::string task_title;
	std::string task_detail;
	size_t total_task_count;
	size_t remain_task_count;
	// key : argument id, value : task argument object.
	std::map<int, DhTaskArg> argument_map;
};
