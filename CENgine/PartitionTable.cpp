#include "PartitionTable.h"
#include "imgui/imgui.h"

PartitionTable::PartitionTable(const Style p_Style, const unsigned int p_PartitionCount, const bool p_HasUEFISignature)
	:
	m_Style(p_Style),
	m_Count(p_PartitionCount),
	m_HasUEFISignature(p_HasUEFISignature)
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

PartitionTable::~PartitionTable()
{
}

void PartitionTable::AddPartition(Partition* p_pPartition)
{
	m_pPartitions.push_back(std::unique_ptr<Partition>(p_pPartition));
}

void PartitionTable::ShowWidget()
{
	std::string stl;

	if (m_Style == PartitionTable::Style::MBR)
	{
		stl = "MBR";
	}
	else if (m_Style == PartitionTable::Style::GPT)
	{
		stl = "GPT";
	}
	else if (m_Style == PartitionTable::Style::RAW)
	{
		stl = "RAW";
	}

	ImGui::Text("Partition Table");
	ImGui::Text("Style: %s", stl.c_str());
	ImGui::Text("Number of Current Partitions: %u", m_Count);
	ImGui::Text("Maximum Number of Partitions: %u", m_MaxPartitionCount);

	for (const auto& p : m_pPartitions)
	{
		p->ShowWidget();
	}
}

const unsigned int PartitionTable::GetPartitionCount() noexcept
{
	return m_Count;
}

const unsigned int PartitionTable::GetMaxPartition() noexcept
{
	return m_MaxPartitionCount;
}