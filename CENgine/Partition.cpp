#include "Partition.h"

Partition::Partition(const PARTITION_INFORMATION_EX& p_PartitionInformation, Style style, const std::string& p_Id)
	:
	m_Id(p_Id),
	m_Style(style),
	m_Number(p_PartitionInformation.PartitionNumber),
	m_Length(p_PartitionInformation.PartitionLength.QuadPart),
	m_StartingOffset(p_PartitionInformation.StartingOffset.QuadPart),
	m_IsRewritePartition(p_PartitionInformation.RewritePartition == 1 ? true : false)
{
	int rpt = -1;
	long long calc = 0;
	for (auto divisor = 1073741824LL; calc < 1 && divisor > 1024; divisor/= 1024, ++rpt)
	{
		calc = m_Length / divisor;
	}

	m_VisualLength = calc;

	if (rpt == 0)
	{
		m_Measurement = std::string("GBs");
	}
	else if (rpt == 1)
	{
		m_Measurement = std::string("MBs");
	}
	else if (rpt == 2)
	{
		m_Measurement = std::string("KBs");
	}
	else if (rpt == 4)
	{
		m_Measurement = std::string("Bs");
	}
}