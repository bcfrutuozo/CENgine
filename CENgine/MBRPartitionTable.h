#pragma once

#include "PartitionTable.h"

class MBRPartitionTable : public PartitionTable
{
public:

	MBRPartitionTable(const unsigned int p_PartitionCount, const bool p_HasUEFISignature);
	void ShowWidget() override;

private:

	unsigned long m_CheckSum;
	unsigned long m_Signature;
};

