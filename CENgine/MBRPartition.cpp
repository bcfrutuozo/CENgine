#include "MBRPartition.h"
#include "imgui/imgui.h"
#include "Utilities.h"

MBRPartition::MBRPartition(const PARTITION_INFORMATION_EX& p_PartitionInformation)
	:
	m_MBRType(MBRPartition::GetTypeByIdentifier(p_PartitionInformation.Mbr.PartitionType)),
	m_HiddenSectors(p_PartitionInformation.Mbr.HiddenSectors),
	m_IsBootIndicator(p_PartitionInformation.Mbr.BootIndicator),
	m_IsRecognizedPartition(p_PartitionInformation.Mbr.RecognizedPartition),
	m_HasProtectiveMBR(m_MBRType.Identifier == 0xEE ? true : false),	// Flag a protective MBR for non GPT platforms (XP)
	Partition(p_PartitionInformation, Partition::Style::MBR, GuidToString(p_PartitionInformation.Mbr.PartitionId))
{
	m_HeaderTitle = StringFormat("MBR Partition %u", m_Number);
}

MBRPartition::~MBRPartition()
{
}

void MBRPartition::ShowWidget()
{
	if (ImGui::CollapsingHeader(m_HeaderTitle.c_str()))
	{
		ImGui::Text("Partition Type: %s", m_MBRType.Name);
		ImGui::Text("Id: %s", m_Id.c_str());
		ImGui::Text("Partition Length: %u %s", m_VisualLength, m_Measurement.c_str());
		ImGui::Text("Starting Offset: %I64d", m_StartingOffset);
		ImGui::Text("HiddenSectors: %Iu", m_HiddenSectors);
		ImGui::Text("Bootable: %s", m_IsBootIndicator ? "Yes" : "No");
		ImGui::Text("Is Recognized Partition: %s", m_IsRecognizedPartition ? "Yes" : "No");
		ImGui::Text("Has Protective MBR: %s", m_HasProtectiveMBR ? "Yes" : "No");
		ImGui::Text("Rewrite Protection: %s", m_IsRewritePartition ? "Yes" : "No");
	}
}

MBRPartitionType MBRPartition::GetTypeByIdentifier(unsigned char c)
{
	for (int i = 0; i < MBRPARTITIONTYPES; i++)
	{
		if (c == Types[i].Identifier)
			return Types[i];
	}

	throw std::runtime_error("Invalid MBR type");
}