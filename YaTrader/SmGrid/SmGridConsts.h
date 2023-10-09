#pragma once
namespace DarkHorse {
	const int DefaultColWidth = 40;
	const int DefaultRowHeight = 21;
	const int WideRowHeight = 25;

	enum class SmHeaderMode {
		/// <summary>
		/// ��� ����
		/// </summary>
		None,
		/// <summary>
		/// �� ����� ����. ����� �Ʒ��� ������
		/// </summary>
		HeaderRowOnly,
		/// <summary>
		/// �� ����� ����. ����� �������θ� ������
		/// </summary>
		HeaderColOnly,
		/// <summary>
		/// ��, �� ��� ��� ����. ��� �켱 ������ �ݵ�� �����ؾ� ��
		/// </summary>
		HeaderBoth
	};

	enum class SmHeaderPriority {
		// �� ��� �켱. �� ����� 1���� �����Ѵ�.
		RowFirst,
		// �� ��� �켱. �� ����� 1���� ���۵ȴ�.
		ColFirst
	};
}