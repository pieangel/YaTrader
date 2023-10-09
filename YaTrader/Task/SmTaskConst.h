#pragma once
#include <map>
#include <string>
#include <any>
#include "SmTaskTypeDef.h"

namespace DarkHorse {
	static std::map<SmTaskType, std::string> SmTaskTitleMap = {
		{SmTaskType::MasterFileDownload, "������������ �������� �ֽ��ϴ�."},
		{SmTaskType::DomesticSymbolCode, "���������ڵ带 �������� �ֽ��ϴ�."},
		{SmTaskType::FileDownload, "������ �ٿ�ε� �ϰ� �ֽ��ϴ�."},
		{SmTaskType::Market, "���� ������ �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolCode, "���� �ڵ带 �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolMaster, "���� ������ �������� �ֽ��ϴ�."},
		{SmTaskType::AccountAsset, "�ڻ� ������ �������� �ֽ��ϴ�."},
		{SmTaskType::AccountProfitLoss, "���º� ������ �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolProfitLoss, "���� ������ �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolQuote, "�ü��� �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolHoga, "ȣ���� �������� �ֽ��ϴ�."},
		{SmTaskType::ChartData, "��Ʈ�����͸� �������� �ֽ��ϴ�."},
		{SmTaskType::AcceptedOrderList, "�����ֹ������ �������� �ֽ��ϴ�."},
		{SmTaskType::FilledOrderList, "ü���ֹ������ �������� �ֽ��ϴ�."},
		{SmTaskType::SymbolPosition, "���� �������� �������� �ֽ��ϴ�."},
		{SmTaskType::RegisterSymbol, "������ ����ϰ� �ֽ��ϴ�."},
		{SmTaskType::RegisterAccount, "���¸� ����ϰ� �ֽ��ϴ�."},
	};
}