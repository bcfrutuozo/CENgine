#pragma once

#include "Partition.h"

#include <memory>
#include <vector>

class PartitionTable
{
public:

	enum class Style
	{
		MBR = 0,
		GPT = 1,
		RAW = 99,
	};

	PartitionTable(const Style p_Style, const unsigned int p_PartitionCount, const bool p_HasUEFISignature);
	virtual ~PartitionTable() = default;

	void AddPartition(Partition* p_pPartition);
	virtual void ShowWidget() = 0;
	const unsigned int GetPartitionCount() noexcept;
	const unsigned int GetMaxPartition() noexcept;

protected:

	Style m_Style;
	bool m_HasUEFISignature;
	unsigned int m_Count;
	unsigned int m_MaxPartitionCount;
	std::vector<std::unique_ptr<Partition>> m_pPartitions;
};