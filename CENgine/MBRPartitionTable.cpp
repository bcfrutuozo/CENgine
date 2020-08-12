#include "MBRPartitionTable.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

MBRPartitionTable::MBRPartitionTable(const unsigned int p_PartitionCount, unsigned long p_CheckSum, unsigned long p_Signature)
	:
	m_CheckSum(p_CheckSum),
	m_Signature(p_Signature),
	m_HasUEFISignature(p_Signature == UEFI_SIGNATURE),
	PartitionTable(Style::MBR, p_PartitionCount)
{ }

void MBRPartitionTable::ShowWidget()
{
	if(ImGui::TreeNode("Partition Table"))
	{
		ImGui::Text("Style: MBR");
		ImGui::Text("CheckSum: %Iu", m_CheckSum);
		ImGui::Text("Signature: %Iu", m_Signature);
		ImGui::Text("Has UEFI Signature: %s", m_HasUEFISignature ? "Yes" : "No");
		ImGui::Text("Number of Current Partitions: %u", m_Count);
		ImGui::Text("Maximum Number of Partitions: %u", m_MaxPartitionCount);

		for (const auto& p : m_pPartitions)
		{
			p->ShowWidget();
		}

		ImGui::TreePop();
	}
}
