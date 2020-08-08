#pragma once

#include "PartitionTable.h"

class GPTPartitionTable : public PartitionTable
{
public:

	GPTPartitionTable(unsigned int p_PartitionCount, const std::string p_DiskId, long long p_StartingUsableOffset, long long p_UsableLength);
	void ShowWidget() override;
private:

    std::string m_DiskId;
    long long m_StartingUsableOffset;
    long long m_UsableLength;
};

