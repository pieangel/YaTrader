#pragma once
#include <atlstr.h>

const CString DefAccountList = "g11004.AQ0101%";
const CString DEF_AVAILABLE_CODE_LIST = _T("g11002.DQ1211&");
// 국내
const CString DEF_TR_CODE = "g11002.DQ0622&";
const CString DEF_FID_CODE = "s20001";
const CString DEF_ORD_CODE_NEW = "g12001.DO1601&";
const CString DEF_ORD_CODE_MOD = "g12001.DO1901&";
const CString DEF_ORD_CODE_CNL = "g12001.DO1701&";

const CString DEF_ORD_CODE_NEW_CME = "g12001.DO2201&";	//CME 신규주문
const CString DEF_ORD_CODE_CNL_CME = "g12001.DO2001&";	//CME 취소
const CString DEF_ORD_CODE_MOD_CME = "g12001.DO2101&";	//CME 정정(가격,조건)주문

const CString DEF_CHE_LIST = "g11002.DQ0107&";
const CString DEF_MICHE_LIST = "g11002.DQ0104&";

const CString DefPutOrder = "g12001.DO1601&";
const CString DefModifyOrder = "g12001.DO1901&";
const CString DefCancelOrder = "g12001.DO1701&";
const CString DefPutCmeOrder = "g12001.DO2201&";	//CME 신규주문
const CString DefModifyCmeOrder = "g12001.DO2001&";	//CME 취소
const CString DefCancelCmeOrder = "g12001.DO2101&";	//CME 정정(가격,조건)주문



const CString DefFilledHistory = "g11002.DQ0107&";
const CString DefOutstandingHistory = "g11002.DQ0110&";
const CString DefOutstanding = "g11002.DQ1305&";
const CString DefCmeAcceptedHistory = "g11002.DQ0116&";
const CString DefCmeFilledHistory = "g11002.DQ0119&";
const CString DefCmeOutstandingHistory = "g11002.DQ0122&";
const CString DefCmeOutstanding = "g11002.DQ1306&";
const CString DefCmeAsset = "g11002.DQ0125&";
const CString DefCmePureAsset = "g11002.DQ1303&";
const CString DefAsset = "g11002.DQ0217&";
const CString DefDeposit = "g11002.DQ0242&";
const CString DefDailyProfitLoss = "g11002.DQ0502&";
const CString DefFilledHistoryTable = "g11002.DQ0509&";
const CString DefAccountProfitLoss = "g11002.DQ0521&";
const CString DefSymbolCode = "g11002.DQ0622&";
const CString DefTradableCodeTable = "g11002.DQ1211&";
const CString DefApiCustomerProfitLoss = "g11002.DQ1302&";
const CString DefChartData = "v90003";
const CString DefCurrentQuote = "l41600";
const CString DefDailyQuote = "l41601";
const CString DefTickQuote = "l41602";
const CString DefSecondQutoe = "l41603";
const CString DefDmSymbolMaster = "s20001";
const CString DefStockFutureSymbolMaster = "s31001";
const CString DefIndustryMaster = "s10001";
const CString DefServerTime = "o44011";

// 해외
const CString DEF_HW_JANGO = "g11004.AQ0605%";
const CString DEF_HW_FID_CODE = "o51000";
const CString DEF_HW_ORD_CODE_NEW = "g12003.AO0401%";
const CString DEF_HW_ORD_CODE_MOD = "g12003.AO0402%";
const CString DEF_HW_ORD_CODE_CNL = "g12003.AO0403%";
const CString DEF_HW_MSTINFO = "o51211";
const CString DefAbSymbolMaster = "o51210";
const CString DefAbSymbolQuote = "o51000";
// 해외선물 호가
const CString DefAbSymbolHoga = "o51010";
// 계좌별 자산
const CString DefAbAsset = "g11004.AQ0605%";
// 계좌별 손익
const CString DefAbAccountProfitLoss = "g11004.AQ0607%";
// 종목별 손익
const CString DefAbSymbolProfitLoss = "g11004.AQ0404%";
// 미결제
const CString DefAbSymbolPosition = "g11004.AQ0403%";
// 미체결
const CString DefAbAccepted = "g11004.AQ0401%";
// 체결
const CString DefAbFilled = "g11004.AQ0402%";
// 체결2
const CString DefAbFilled2 = "g11004.AQ0451%";
// 해외 신규 주문
const CString DefAbOrderNew = "g12003.AO0401%";
// 해외 정정 주문
const CString DefAbOrderMod = "g12003.AO0402%";
// 해외 취소 주문
const CString DefAbOrderCancel = "g12003.AO0403%";
// 해외 차트 데이터
const CString DefAbChartData = "o51200";
// 해외 차트 데이터
const CString DefAbsChartData2 = "o44005";
// 해외 체결 상세 정보
const CString DefAbFilledDetail = "g11004.AQ0408%";

const CString DefDmSymbolMasterFile = "v90001";

// FX마진	//@lhe 2012.06.22
const CString DEF_FX_JANGO = "g11004.AQ0901%";		// 자산내역
const CString DEF_FX_FID_CODE = "x00001";				// FX마스터
const CString DEF_FX_ORD_CODE_NEW = "g12003.AO0501%";	// 예약,시장가,청산주문
const CString DEF_FX_ORD_CODE_MOD = "g12003.AO0502%";	// 정정
const CString DEF_FX_ORD_CODE_CNL = "g12003.AO0503%";	// 취소주문

// 계좌별 자산
const CString DefDmAsset = "g11002.DQ0217&";

const CString DefDmAccountProfitLoss = "g11002.DQ1302&";
const CString DefDmAccepted = "g11002.DQ0104&";
const CString DefDmSymbolPosition = "g11002.DQ0110&";

// 종목별 손익
const CString DefDmSymbolProfitLoss = "g11002.DQ1302&";


