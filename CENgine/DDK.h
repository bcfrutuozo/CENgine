#pragma once

#include <ntddscsi.h>

#ifndef StorageDeviceTrimProperty
#define StorageDeviceTrimProperty 8
#endif

//#ifndef DEVICE_TRIM_DESCRIPTOR
//typedef struct _DEVICE_TRIM_DESCRIPTOR
//{
//	DWORD   Version;
//	DWORD   Size;
//	BOOLEAN TrimEnabled;
//} DEVICE_TRIM_DESCRIPTOR, * PDEVICE_TRIM_DESCRIPTOR;
//#endif
//
//
//#ifndef StorageDeviceSeekPenaltyProperty
//#define StorageDeviceSeekPenaltyProperty 7
//#endif
//
//#ifndef DEVICE_SEEK_PENALTY_DESCRIPTOR
//typedef struct _DEVICE_SEEK_PENALTY_DESCRIPTOR {
//	DWORD   Version;
//	DWORD   Size;
//	BOOLEAN IncursSeekPenalty;
//} DEVICE_SEEK_PENALTY_DESCRIPTOR, * PDEVICE_SEEK_PENALTY_DESCRIPTOR;
//#endif


struct ATAIdentifyDeviceQuery
{
	ATA_PASS_THROUGH_EX header;
	WORD data[256];
};