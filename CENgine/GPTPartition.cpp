#include "GPTPartition.h"
#include "imgui/imgui.h"

GPTPartition::GPTPartition(const PARTITION_INFORMATION_EX& p_PartitionInformation)
	:
	m_Name(ToNarrow(p_PartitionInformation.Gpt.Name)),
	m_GPTType(GPTPartition::GetTypeByIdentifier(p_PartitionInformation.Gpt.PartitionType)),
	m_Attributes(p_PartitionInformation.Gpt.Attributes),
	Partition(p_PartitionInformation, Partition::Style::GPT, GuidToString(p_PartitionInformation.Gpt.PartitionId))
{
	m_HeaderTitle = StringFormat("GPT Partition %u", m_Number);
}

GPTPartition::~GPTPartition()
{
}

void GPTPartition::ShowWidget()
{
	if (ImGui::CollapsingHeader(m_HeaderTitle.c_str()))
	{
		ImGui::Text("Partition Type: %s", m_GPTType.Name);
		ImGui::Text("Id: %s", m_Id.c_str());
		ImGui::Text("Partition Length: %u %s", m_VisualLength, m_Measurement.c_str());
		ImGui::Text("Starting Offset: %I64d", m_StartingOffset);
		ImGui::Text("Attributes: %I64u", m_Attributes);
		ImGui::Text("Rewrite Protection: %s", m_IsRewritePartition ? "Yes" : "No");
	}
}

//bool GPTPartition::IsLDMPartition()
//{
//	if (m_GPTType.Identifier == GUID{ 0x5808C8AA, 0x7E8F, 0x42E0, 0x85, 0xD2, 0xE1, 0xE9, 0x04, 0x34, 0xCF, 0xB3 })
//	{
//		return true;
//	}
//
//	return false;
//}

GPTPartitionType GPTPartition::GetTypeByIdentifier(GUID guid)
{
	for (int i = 0; i < GPTPARTITIONTYPES; i++)
	{
		if (guid == Types[i].Identifier)
			return Types[i];
	}
}