#include "PartitionTable.h"
#include "imgui/imgui.h"

PartitionTable::PartitionTable(const Style p_Style, const unsigned int p_PartitionCount)
	:
	m_Style(p_Style),
	m_Count(p_PartitionCount)
{
	if (m_Style == Style::MBR || m_Style == Style::RAW)
	{
		m_MaxPartitionCount = 4;
	}
	else if (m_Style == Style::GPT)
	{
		m_MaxPartitionCount = 128;
	}
}

void PartitionTable::AddPartition(Partition* p_pPartition)
{
	m_pPartitions.push_back(std::unique_ptr<Partition>(p_pPartition));
}

const unsigned int PartitionTable::GetPartitionCount() noexcept
{
	return m_Count;
}

const unsigned int PartitionTable::GetMaxPartition() noexcept
{
	return m_MaxPartitionCount;
}