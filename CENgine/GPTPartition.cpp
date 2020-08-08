#include "GPTPartition.h"
#include "imgui/imgui.h"

GPTPartition::GPTPartition(const PARTITION_INFORMATION_EX& p_PartitionInformation)
	:
	Partition(p_PartitionInformation, Partition::Style::GPT, GuidToString(p_PartitionInformation.Gpt.PartitionId))
{
	m_Name = ToNarrow(p_PartitionInformation.Gpt.Name);
	m_GPTType = GPTPartition::GetTypeByIdentifier(p_PartitionInformation.Gpt.PartitionType);
	m_Attributes = p_PartitionInformation.Gpt.Attributes;
}

GPTPartition::~GPTPartition()
{
}

void GPTPartition::ShowWidget()
{
	ImGui::Text("GPT Partition %u", m_Number);
	ImGui::Text("Partition Type: %s", m_GPTType.Name);
	ImGui::Text("Id: %s", m_Id.c_str());
	ImGui::Text("Partition Length: %u %s", m_VisualLength, m_Measurement.c_str());
	ImGui::Text("Starting Offset: %I64d", m_StartingOffset);
	ImGui::Text("Attributes: %I64u", m_Attributes);
	ImGui::Text("Rewrite Protection: %s", m_IsRewritePartition ? "Yes" : "No");
}

GPTPartitionType GPTPartition::GetTypeByIdentifier(GUID guid)
{
	for (int i = 0; i < GPTPARTITIONTYPES; i++)
	{
		if (guid == Types[i].Identifier)
			return Types[i];
	}
}