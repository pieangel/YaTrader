#include "stdafx.h"
#include "AccountAssetControl.h"
#include "../Util/IdGenerator.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include "../Fund/SmFund.h"
namespace DarkHorse {

AccountAssetControl::AccountAssetControl()
: id_(IdGenerator::get_id())
{

}

AccountAssetControl::~AccountAssetControl()
{

}
void AccountAssetControl::reset()
{
	asset_.entrust_total = 0; // ��Ź���Ѿ�
	asset_.balance = 0; // ��Ź���ܾ�
	asset_.outstanding_deposit = 0; // �̰������ű�
	asset_.order_deposit = 0; // �ֹ����ű�
	asset_.entrust_deposit = 0; // ��Ź���ű�
	asset_.maintenance_margin = 0; // �������ű�
	asset_.trade_profit_loss = 0; // û�����
	asset_.trade_fee = 0; // �����ɼǼ�����
	asset_.open_profit_loss = 0; // �򰡼���
	asset_.open_trust_toal = 0; // �򰡿�Ź�Ѿ�
	asset_.additional_margin = 0; // �߰����ű�
	asset_.order_margin = 0; // �ֹ����ɱݾ�
	asset_.currency = "";
}


void AccountAssetControl::load_position_from_parent_account(const std::string& account_no)
{

}

void AccountAssetControl::load_position_from_fund(const std::string& fund_name)
{

}

void AccountAssetControl::load_asset_from_account(const std::string& account_no)
{
	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	asset_.entrust_total = account->Asset.EntrustTotal; // ��Ź���Ѿ�
	asset_.balance = account->Asset.EntrustTotal; // ��Ź���ܾ�
	asset_.outstanding_deposit = account->Asset.OutstandingDeposit; // �̰������ű�
	asset_.order_deposit = account->Asset.OrderDeposit; // �ֹ����ű�
	asset_.entrust_deposit = account->Asset.EntrustDeposit; // ��Ź���ű�
	asset_.maintenance_margin = account->Asset.MaintenanceMargin; // �������ű�
	asset_.trade_profit_loss = account->Asset.TradeProfitLoss; // û�����
	asset_.trade_fee = account->Asset.Fee; // �����ɼǼ�����
	asset_.open_profit_loss = account->Asset.OpenProfitLoss; // �򰡼���
	asset_.open_trust_toal = account->Asset.OpenTrustTotal; // �򰡿�Ź�Ѿ�
	asset_.additional_margin = account->Asset.AdditionalMargin; // �߰����ű�
	asset_.order_margin = account->Asset.OrderMargin; // �ֹ����ɱݾ�
	asset_.currency = account->Asset.Currency;
}

void AccountAssetControl::load_asset_from_account(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	if (account->is_subaccount()) {
		if (!account->parent_account().lock()) return;
		load_account_asset(account->parent_account().lock());
	}
	else
		load_account_asset(account);
}

void AccountAssetControl::load_account_asset(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	asset_.entrust_total = account->Asset.EntrustTotal; // ��Ź���Ѿ�
	asset_.balance = account->Asset.EntrustTotal; // ��Ź���ܾ�
	asset_.outstanding_deposit = account->Asset.OutstandingDeposit; // �̰������ű�
	asset_.order_deposit = account->Asset.OrderDeposit; // �ֹ����ű�
	asset_.entrust_deposit = account->Asset.EntrustDeposit; // ��Ź���ű�
	asset_.maintenance_margin = account->Asset.MaintenanceMargin; // �������ű�

	//asset_.trade_profit_loss = account->Asset.TradeProfitLoss; // û�����
	//asset_.trade_fee = account->Asset.Fee; // �����ɼǼ�����
	//asset_.open_profit_loss = account->Asset.OpenProfitLoss; // �򰡼���


	asset_.open_trust_toal = account->Asset.OpenTrustTotal; // �򰡿�Ź�Ѿ�
	asset_.additional_margin = account->Asset.AdditionalMargin; // �߰����ű�
	asset_.order_margin = account->Asset.OrderMargin; // �ֹ����ɱݾ�
	asset_.currency = account->Asset.Currency;
}

void AccountAssetControl::set_account(std::shared_ptr<SmAccount> account)
{
	if (!account) return;
	account_ = account;
	load_asset_from_account(account);
}

void AccountAssetControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	auto first_account = fund_->get_first_account();
	if (!first_account) {
		reset();  
		return;
	}
	load_asset_from_account(first_account);
}

}