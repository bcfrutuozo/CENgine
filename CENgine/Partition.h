#pragma once

#include "PartitionType.h"
#include "Utilities.h"

#include <Windows.h>
#include <winioctl.h>

static constexpr int MBRPARTITIONTYPES = 98;
static constexpr int GPTPARTITIONTYPES = 138;

class Partition
{
public:

	enum class Style
	{
		MBR = 0,
		GPT = 1,
		RAW = 99,
	};

	Partition(const PARTITION_INFORMATION_EX& p_PartitionInformation, Style style, const std::string& p_Id);
	virtual ~Partition() = default;
	virtual void ShowWidget() = 0;

protected:

	std::string m_Id;
	std::string m_HeaderTitle;
	unsigned int m_VisualLength;
	std::string m_Measurement;
	unsigned long m_Number;
	long long m_Length;
	Style m_Style;
	long long m_StartingOffset;
	bool m_IsRewritePartition;
};