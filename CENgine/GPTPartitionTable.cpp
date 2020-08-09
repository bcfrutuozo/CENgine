#include "GPTPartitionTable.h"
#include "imgui/imgui.h"

GPTPartitionTable::GPTPartitionTable(unsigned int p_PartitionCount, const std::string p_DiskId, long long p_StartingUsableOffset, long long p_UsableLength)
	:
	m_DiskId(p_DiskId),
	m_StartingUsableOffset(p_StartingUsableOffset),
	m_UsableLength(p_UsableLength),
	PartitionTable(Style::GPT, p_PartitionCount)
{	}

void GPTPartitionTable::ShowWidget()
{
	if (ImGui::CollapsingHeader("Partition Table"))
	{
		ImGui::Text("Style: GPT");
		ImGui::Text("Disk Id: %s", m_DiskId.c_str());
		ImGui::Text("Starting Usable Offset: %I64d", m_StartingUsableOffset);
		ImGui::Text("Usable Length: %I64d", m_UsableLength);
		ImGui::Text("Number of Current Partitions: %u", m_Count);
		ImGui::Text("Maximum Number of Partitions: %u", m_MaxPartitionCount);

		for (const auto& p : m_pPartitions)
		{
			p->ShowWidget();
		}
	}
}
