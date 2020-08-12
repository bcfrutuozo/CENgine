#include "PartitionTable.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

PartitionTable::PartitionTable(const Style p_Style, const unsigned int p_PartitionCount)
	:
	m_Style(p_Style),
	m_Count(p_PartitionCount),
	m_MaxPartitionCount((p_Style == Style::MBR || p_Style == Style::RAW) ? 4 : 128)
{	}

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