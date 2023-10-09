#pragma once

namespace DarkHorse {
	enum class SmTaskType
	{
		None,
		MasterFileDownload, // 마스터 파일 다운 로드
		FileDownload, // 해외 마스터 파일 자동 다운로드.
		Market, // 시장 정보
		SymbolCode, // 종목 코드
		SymbolMaster, // 종목 정보
		DmSymbolMaster, // 국내 종목 정보
		AccountAsset, // 자산 정보
		AccountProfitLoss, // 계좌 손익
		SymbolProfitLoss, // 종목 손익
		SymbolQuote, // 시세 
		SymbolHoga, // 호가
		ChartData, // 차트 데이터
		AcceptedOrderList, // 접수확인 주문 목록
		FilledOrderList, // 체결 확인 주문 목록
		SymbolPosition, // 포지션
		RegisterSymbol,
		RegisterAccount,
		DomesticSymbolCode // 국내 상품 코드 정보
	};
}