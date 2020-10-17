#pragma once

#include "CENgineWin.h"
#include <guiddef.h>

struct MBRPartitionType
{
	unsigned char Identifier;
	const char* Name;
};

struct GPTPartitionType
{
	GUID Identifier;
	const char* Name;
};