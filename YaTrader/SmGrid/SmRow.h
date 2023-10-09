#pragma once
#include <map>
#include <memory>
namespace DarkHorse {
	class SmCell;
	class SmRow
	{
	public:
		SmRow(const int& row_index) : _RowIndex(row_index) {};
		~SmRow() {};
		std::shared_ptr<SmCell> FindCell(const int& col);
		const std::map<int, std::shared_ptr<SmCell>>& GetCellList() {
			return _ColCellMap;
		}
		int RowIndex() const { return _RowIndex; }
		void RowIndex(int val) { _RowIndex = val; }
		int Height() const { return _Height; }
		void Height(int val) { _Height = val; }
		void AddCell(const std::shared_ptr<SmCell>& cell);
	private:
		int _RowIndex{ -1 };
		int _Height{ 0 };
		// key : col, value : SmCell object.
		std::map<int, std::shared_ptr<SmCell>> _ColCellMap;
	};
}

