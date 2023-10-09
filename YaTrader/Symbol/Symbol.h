#pragma once
#include <string>
namespace DarkHorse {
	struct Symbol {
		/// <summary>
		/// �ɺ� �ڵ�
		/// </summary>
		std::string symbol_code;
		/// <summary>
		/// �ɺ� Ǯ�ڵ�
		/// </summary>
		std::string full_code;
		/// <summary>
		/// ���� �̸�
		/// </summary>
		std::string name_en;
		/// <summary>
		/// �ѱ� �̸�
		/// </summary>
		std::string name_kr;
		/// <summary>
		/// ���� �ϼ�
		/// </summary>
		int remain_days;
		/// <summary>
		/// ���� �ŷ���
		/// </summary>
		std::string last_trade_day;
		/// <summary>
		/// �ֻ��� 1ȣ�� 
		/// </summary>
		std::string high_limit_price;
		/// <summary>
		/// ������ 1ȣ��
		/// </summary>
		std::string low_limit_price;
		/// <summary>
		/// ������ ����
		/// </summary>
		std::string preday_close;
		/// <summary>
		/// ���ذ�
		/// </summary>
		std::string standard_price;
		/// <summary>
		/// ��簡
		/// </summary>
		std::string strike;
		/// <summary>
		/// 0 : future, 1 : atm , 2 : itm, 3 : otm
		/// </summary>
		int atm_type;
		/// <summary>
		/// 1 : �ֱٿ���, ���� ��������, 2 : 2°����, 3���.
		/// </summary>
		int recent_month;
		/// <summary>
		/// ������
		/// </summary>
		std::string expire_day;
		/// <summary>
		/// ���̵�
		/// </summary>
		int id{ 0 };
		/// <summary>
		/// �¼�
		/// </summary>
		int seung_su{ 250000 };
		/// <summary>
		/// �Ҽ��� �ڸ���
		/// </summary>
		int decimal{ 2 };
		/// <summary>
		/// ��� ũ��
		/// </summary>
		double contract_size{ 0.05 };
		/// <summary>
		/// ƽ��ġ
		/// </summary>
		double tick_value{ 12500 };
		/// <summary>
		/// ƽũ��
		/// </summary>
		double tick_size{ 0.05 };
		/// <summary>
		/// ���� �̸�
		/// </summary>
		std::string market_name;
		/// <summary>
		/// ��ǰ �ڵ�
		/// </summary>
		std::string product_code;
		/// <summary>
		/// �� ��ü �ŷ��� 
		/// </summary>
		int total_volume{ 0 };
		/// <summary>
		/// ������ �ŷ���
		/// </summary>
		int preday_volume{ 0 };
		/// <summary>
		/// �ܰ�
		/// </summary>
		std::string deposit;
		/// <summary>
		/// ���� �ð�
		/// </summary>
		std::string start_time;
		/// <summary>
		/// ���� �ð�
		/// </summary>
		std::string end_time;
		/// <summary>
		/// ���/�϶���
		/// </summary>
		std::string preday_updown_rate;
		/// <summary>
		/// ��ȭǥ��
		/// </summary>
		std::string currency;
		/// <summary>
		/// �ŷ���
		/// </summary>
		std::string exchange;
	};
}