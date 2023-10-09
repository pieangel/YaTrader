#pragma once
#include<string>
#include <map>
#include "../Json/json.hpp"

enum class DhTaskType
{
	// �ʱ� ����
	None = 0,
	// ���� ����
	AccountProfitLoss,
	// Chart Data
	SymbolChartData,
	RegisterSymbol,
	// ���� ���� �ɺ� 
	DmJisuFutureSymbol,
	// ���� �ɼ� �ɺ� 
	DmJisuOptionSymbol,
	// ��ǰ ���� �ɺ� 
	DmCommodityFutureSymbol,
	// �̴� �ڽ��� ���� �ɺ� 
	DmMiniFutureSymbol,
	// �̴� �ڽ��� ���� �ɼ� �ɺ� 
	DmMiniOptionSymbol,
	// EFT �ɺ� ����
	DmEtfSymbol,
	/// <summary>
	/// ��Ŭ�� �ɼ� �ɺ� ����
	/// </summary>
	DmWeeklyOpSymbol,
	// �ؿ� ���� �ɺ� ��� ����
	AbroadSymbol,
	AbSymbolCode, // �ɺ� �ڵ�
	ABProductRemain, // ���� �ܰ�� �� ����
	AbAccountOrder, // ���º� �ֹ� ����
	AbSymbolMaster, // �ؿ� ���� ����
	AbSymbolQuote, // �ؿ� ���� �ü�
	AbSymbolHoga, // �ؿ� ���� ȣ��
	AbSymbolChartData, // �ؿ� ���� ��Ʈ ������
	AbAccountAsset, // �ؿ� ���� 
	AbAccountProfitLoss, // �ؿ� ���� ����
	AbSymbolProfitLoss, // �ؿ� ���� ����
	AbSymbolPosition, // �ؿ� ���� �ܰ�
	AbAcceptedOrderList, // �ؿ� ��ü�� �ֹ�
	AbFilledOrderList, // �ؿ� ü�� �ֹ�
	AbFilledDetail, // �ؿ� ü�� �ֹ�
	AbSymbolMasterFileDownload, // �ɺ� ������ ���� �ٿ�ε�

	DmAcceptedOrderList, // ����Ȯ�� ����
	DmFilledHistory, // ü�᳻��
	DmSymbolPosition, // �ܰ���
	DmSymbolProfitLoss, // ���� ���� ����
	DmOutstanding, // �����ܰ�
	DmCmeAcceptedHistory, // cme ����Ȯ�� ����
	DmCmeFilledHistory, // cme ü�᳻��
	DmCmeOutstandingHistory, // cme �ܰ���
	DmCmeOutstanding, // cme �ܰ�
	DmCmeAsset, // cme �ڻ�
	DmCmePureAsset, // cme ���ڻ�
	DmAccountAsset, // �ڻ�
	DmDeposit, // ���� ����
	DmDailyProfitLoss, // ���� ����
	DmFilledHistoryTable, // ü�� ����
	DmAccountProfitLoss, // ���º� ����
	DmSymbolCode, // �ɺ� �ڵ�
	DmTradableCodeTable, // �ŷ����� ����ǥ
	DmApiCustomerProfitLoss, // �� ���� ��Ȳ
	DmChartData, // ��Ʈ ������
	DmCurrentQuote, // ���� �ü�
	DmDailyQuote, // �Ϻ� �ü�
	DmTickQuote, // ƽ �ü�
	DmSecondQutoe, // ��� �ü�
	DmSymbolMaster, // �ɺ� ������
	DmStockFutureSymbolMaster, // ���Ǽ��� �ɺ� ������
	DmIndustryMaster, // ����� ������
	DmTaskComplete, // �۾� �Ϸ�
	DmRecentMonthSymbolMaser, // �ֱٿ� �ɺ� ������
	DmOrderNew, // �ű��ֹ�
	DmOrderChange, // ���� �ֹ�
	DmOrderCancel, // ��� �ֹ�
	DmAccountFeeInfoStep1, // ���� ����
	DmAccountFeeInfoStep2, // ���� ����
	DmSymbolFileDownload, // �ɺ����� �ٿ�ε�
	DmSymbolMasterFileDownload // �ɺ� ������ ���� �ٿ�ε�
};

struct DhTaskArg {
	// ��û ���̵�
	int argument_id = 0;
	// ��û�ߴ��� ����
	bool requested = false;
	// �޾Ҵ��� ����
	bool received = false;
	DhTaskType task_type = DhTaskType::None;
	// �۾� ����
	std::string description;
	// �۾� ����
	std::string detail_task_description;
	// �Ű����� ��
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
