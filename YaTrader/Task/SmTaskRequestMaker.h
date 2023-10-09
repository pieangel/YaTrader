#pragma once
#include "SmTaskInfo.h"
#include "../Chart/SmChartConsts.h"
#include <string>
#include <any>
typedef std::map<std::string, std::any> task_arg;

namespace DarkHorse {
	struct SmTaskInfo;
	/// <summary>
	/// ��û ��ü�� �����.
	/// </summary>
	class SmTaskRequestMaker
	{
	public:
		SmTaskRequestMaker() {};
		~SmTaskRequestMaker() = default;
		// �ʱ� �۾� ����� �����.
		static void MaketInitialBatchTask(std::vector<std::shared_ptr<SmTaskInfo>>& task_list);
		// ���� ���� �ɺ� ����� �����.
		static std::shared_ptr<SmTaskInfo> MakeDomesticSymbolCodeRequest();
		/// <summary>
		/// ���� ��û������ �����.
		/// </summary>
		/// <param name="task_type">���� ����</param>
		/// <returns>���� ��ü. ������ ���� �ڼ��� ������ ��� �ִ�.</returns>
		static std::shared_ptr<SmTaskInfo> MakeTaskRequest(const SmTaskType& task_type);
		/// <summary>
		/// ���� ���� �ٿ�ε带 ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo>  MakeFileDownloadRequest() ;
		static std::shared_ptr<SmTaskInfo> MakeMasterFileDownloadRequest(const std::string& file_name);
		/// <summary>
		/// ���� ������ ��û�Ѵ�. 
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolMasterRequest();
		static task_arg MakeSymbolMasterRequest(const std::string& symbol_code);
		/// <summary>
		/// �ü� ������ ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolQuoteRequest();
		static task_arg MakeSymbolQuoteRequest(const std::string& symbol_code);
		/// <summary>
		/// ȣ�� ������ ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolHogaRequest();
		static task_arg MakeSymbolHogaRequest(const std::string& symbol_code);
		/// <summary>
		/// �ڻ� ������ ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAccountAssetRequest();
		static task_arg MakeAccountAssetRequest(
			const std::string& account_no, 
			const std::string& pwd,
			const std::string& type);
		/// <summary>
		/// ���º� ������ ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAccountProfitLossRequest();
		/// <summary>
		/// ���� �����ǰ� ������ ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeSymbolProfitLossRequest();
		/// <summary>
		/// ����Ȯ�ε� �ֹ� ����� ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeAcceptedOrderRequest();
		/// <summary>
		/// ü��� �ֹ� ����� ��û�Ѵ�.
		/// </summary>
		/// <returns></returns>
		static std::shared_ptr<SmTaskInfo> MakeFilledOrderRequest();
		/// <summary>
		/// ��Ʈ ������ ��û�� �����.
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

