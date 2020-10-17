#pragma once

#include "CENgineWin.h"

#include <ntddscsi.h>

#ifndef StorageDeviceTrimProperty
#define StorageDeviceTrimProperty 8
#endif

struct ATAIdentifyDeviceQuery
{
	ATA_PASS_THROUGH_EX header;
	WORD data[256];
};