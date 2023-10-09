#pragma once
namespace DarkHorse {
	const int DefaultColWidth = 40;
	const int DefaultRowHeight = 21;
	const int WideRowHeight = 25;

	enum class SmHeaderMode {
		/// <summary>
		/// 헤더 없음
		/// </summary>
		None,
		/// <summary>
		/// 행 헤더만 존재. 헤더가 아래로 전개됨
		/// </summary>
		HeaderRowOnly,
		/// <summary>
		/// 열 헤더만 존대. 헤더가 우측으로만 전개됨
		/// </summary>
		HeaderColOnly,
		/// <summary>
		/// 행, 열 헤더 모두 존재. 헤더 우선 순위를 반드시 설정해야 함
		/// </summary>
		HeaderBoth
	};

	enum class SmHeaderPriority {
		// 행 헤더 우선. 열 헤더는 1부터 시작한다.
		RowFirst,
		// 열 헤더 우선. 행 헤더는 1부터 시작된다.
		ColFirst
	};
}