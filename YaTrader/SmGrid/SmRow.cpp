#include "stdafx.h"
#include "SmRow.h"
#include "SmCell.h"

std::shared_ptr<DarkHorse::SmCell> DarkHorse::SmRow::FindCell(const int& col)
{
	auto found = _ColCellMap.find(col);
	if (found != _ColCellMap.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmRow::AddCell(const std::shared_ptr<SmCell>& cell)
{
	if (!cell) return;

	_ColCellMap[cell->Col()] = cell;
}
