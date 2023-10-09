#pragma once
#include "SmTaskInfo.h"
#include "../Chart/SmChartConsts.h"
#include <string>
#include <any>
typedef std::map<std::string, std::any> task_arg;

namespace DarkHorse {
	struct SmTaskInfo;
	/// <summary>
	/// 요청 객체를 만든다.
	/// </summary>
	class SmTaskRequestMaker
	{
	public:
		SmTaskRequestMaker() {};
		~SmTaskRequestMaker() = default;
		// 초기 작업 목록을 만든다.
		static void MaketInitialBatchTask(std::vector<std::shared_ptr<SmTaskInfo>>& task_list);
		// 국내 선물 심볼 목록을 만든다.
		static std::shared_ptr<SmTaskInfo> MakeDomesticSymbolCodeRequest();
		/// <summary>
		/// 업무 요청사항을 만든다.
		/// </summary>
		/// <param name="task_type">업무 유형</param>
		/// <returns>업무 객체. 업무에 대한 자세한 정보가 들어 있다.</returns>
		static std::shared_ptr<SmTaskInfo> MakeTaskRequest(const SmTaskType& task_type);
		/// <summary>
		/// 종목 파일 다운로드를 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo>  MakeFileDownloadRequest() ;
		static std::shared_ptr<SmTaskInfo> MakeMasterFileDownloadRequest(const std::string& file_name);
		/// <summary>
		/// 종목 정보를 요청한다. 
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolMasterRequest();
		static task_arg MakeSymbolMasterRequest(const std::string& symbol_code);
		/// <summary>
		/// 시세 정보를 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolQuoteRequest();
		static task_arg MakeSymbolQuoteRequest(const std::string& symbol_code);
		/// <summary>
		/// 호가 정보를 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolHogaRequest();
		static task_arg MakeSymbolHogaRequest(const std::string& symbol_code);
		/// <summary>
		/// 자산 정보를 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAccountAssetRequest();
		static task_arg MakeAccountAssetRequest(
			const std::string& account_no, 
			const std::string& pwd,
			const std::string& type);
		/// <summary>
		/// 계좌별 손익을 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAccountProfitLossRequest();
		/// <summary>
		/// 종목별 포지션과 손익을 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolProfitLossRequest();
		/// <summary>
		/// 접수확인된 주문 목록을 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAcceptedOrderRequest();
		/// <summary>
		/// 체결된 주문 목록을 요청한다.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeFilledOrderRequest();
		/// <summary>
		/// 차트 데이터 요청을 만든다.
		/// </summary>
		/// <param name="symbol_code"></param>
		/// <param name="chart_type"></param>
		/// <param name="cycle"></param>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeChartDataRequest(const std::string& symbol_code, SmChartType chart_type, const int& cycle, const int& count);

		static std::shared_ptr<SmTaskInfo> MakeDefaultChartDataRequest(const int& count);

		static std::shared_ptr<SmTaskInfo> MakeDefaultChartDataRequest(std::vector<std::shared_ptr<SmTaskInfo>>& task_list, const int& count);

		static std::shared_ptr<SmTaskInfo> MakeSymbolRegisterRequest(const std::string& symbol_code, const bool& reg);
		static std::shared_ptr<SmTaskInfo> MakeAccountRegisterRequest(const std::string& account_no, const bool& reg);
	};
}

