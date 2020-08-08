#include "MBRPartitionTable.h"
#include "imgui/imgui.h"

MBRPartitionTable::MBRPartitionTable(const unsigned int p_PartitionCount, const bool p_HasUEFISignature)
	:
	PartitionTable(Style::MBR, p_PartitionCount, p_HasUEFISignature)
{ }

void MBRPartitionTable::ShowWidget()
{
	ImGui::Text("Partition Table");
	ImGui::Text("Style: MBR");
	ImGui::Text("CheckSum: %u", m_CheckSum);
	ImGui::Text("Signature: %u", m_Signature);
	ImGui::Text("Has UEFI Signature: &s", m_HasUEFISignature ? "Yes" : "No");
	ImGui::Text("Number of Current Partitions: %u", m_Count);
	ImGui::Text("Maximum Number of Partitions: %u", m_MaxPartitionCount);

	for (const auto& p : m_pPartitions)
	{
		p->ShowWidget();
	}
}
