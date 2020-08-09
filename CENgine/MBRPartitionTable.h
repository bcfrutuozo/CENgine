#pragma once

#include "PartitionTable.h"

#define UEFI_SIGNATURE 0x49464555	// 'U', 'E', 'F', 'I', as a 32 bit little endian longword

class MBRPartitionTable : public PartitionTable
{
public:

	MBRPartitionTable(const unsigned int p_PartitionCount, unsigned long p_CheckSum, unsigned long p_Signature);
	void ShowWidget() override;

private:

	unsigned long m_CheckSum;
	unsigned long m_Signature;
	bool m_HasUEFISignature;
};

