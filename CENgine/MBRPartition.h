#pragma once

#include "Partition.h"

class MBRPartition : public Partition
{
public:

	MBRPartition(const PARTITION_INFORMATION_EX& p_Layout);
	~MBRPartition();
	void ShowWidget() override;
	MBRPartitionType GetTypeByIdentifier(unsigned char c);

private:

	MBRPartitionType m_MBRType;
	unsigned long m_HiddenSectors;
	bool m_IsBootIndicator;
	bool m_IsRecognizedPartition;
	bool m_HasProtectiveMBR; // Flag a protective MBR for non GPT platforms (XP)

	static constexpr MBRPartitionType Types[MBRPARTITIONTYPES] =
	{
		{ 0x00, "Empty Entry" },
		{ 0x01, "FAT12" },
		{ 0x02, "XENIX root" },
		{ 0x03, "XENIX usr" },
		{ 0x04, "Small FAT16" },
		{ 0x05, "Extended" },
		{ 0x06, "FAT16" },
		{ 0x07, "NTFS" },
		{ 0x08, "AIX" },
		{ 0x09, "AIX Bootable" },
		{ 0x0A, "OS/2 Boot Manager" },
		{ 0x0B, "FAT32" },
		{ 0x0C, "FAT32 LBA" },
		{ 0x0E, "FAT16 LBA" },
		{ 0x0F, "Extended LBA" },
		{ 0x10, "OPUS" },
		{ 0x11, "Hidden FAT12" },
		{ 0x12, "Compaq Diagnostics" },
		{ 0x14, "Hidden Small FAT16" },
		{ 0x16, "Hidden FAT16" },
		{ 0x17, "Hidden NTFS" },
		{ 0x18, "AST SmartSleep" },
		{ 0x1B, "Hidden FAT32" },
		{ 0x1C, "Hidden FAT32 LBA" },
		{ 0x1E, "Hidden FAT16 LBA" },
		{ 0x24, "NEC DOS" },
		{ 0x27, "Hidden NTFS WinRE" },
		{ 0x39, "Plan 9" },
		{ 0x3C, "PMagic recovery" },
		{ 0x40, "Venix 80286" },
		{ 0x41, "PPC PReP Boot" },
		{ 0x42, "SFS" },
		{ 0x4D, "QNX4.x" },
		{ 0x4E, "QNX4.x 2nd Partition" },
		{ 0x4F, "QNX4.x 3rd Partition" },
		{ 0x50, "OnTrack DM" },
		{ 0x51, "OnTrackDM6 Aux1" },
		{ 0x52, "CP/M" },
		{ 0x53, "OnTrackDM6 Aux3" },
		{ 0x54, "OnTrack DM6" },
		{ 0x55, "EZ Drive" },
		{ 0x56, "Golden Bow" },
		{ 0x5C, "Priam Edisk" },
		{ 0x61, "SpeedStor" },
		{ 0x63, "GNU HURD/SysV" },
		{ 0x64, "Netware 286" },
		{ 0x65, "Netware 386" },
		{ 0x70, "DiskSec MultiBoot" },
		{ 0x75, "PC/IX" },
		{ 0x80, "Minix <1.4a" },
		{ 0x81, "Minix >1.4b" },
		{ 0x82, "Linux Swap" },
		{ 0x83, "Linux" },
		{ 0x84, "OS/2 Hidden C:" },
		{ 0x85, "Linux Extended" },
		{ 0x86, "NTFS Volume Set" },
		{ 0x87, "NTFS Volume Set" },
		{ 0x88, "Linux Plaintext" },
		{ 0x8E, "Linux LVM" },
		{ 0x93, "Amoeba" },
		{ 0x94, "Amoeba BBT" },
		{ 0x9F, "BSD/OS" },
		{ 0xA0, "Thinkpad Hibernation" },
		{ 0xA5, "FreeBSD" },
		{ 0xA6, "OpenBSD" },
		{ 0xA7, "NeXTSTEP" },
		{ 0xA8, "Darwin UFS" },
		{ 0xA9, "NetBSD" },
		{ 0xAB, "Darwin Boot" },
		{ 0xAF, "HFS/HFS+" },
		{ 0xB7, "BSDI" },
		{ 0xB8, "BSDI Swap" },
		{ 0xBB, "Boot Wizard Hidden" },
		{ 0xBE, "Solaris Boot" },
		{ 0xBF, "Solaris" },
		{ 0xC1, "DRDOS/2 FAT12" },
		{ 0xC4, "DRDOS/2 smFAT16" },
		{ 0xC6, "DRDOS/2 FAT16" },
		{ 0xC7, "Syrinx" },
		{ 0xDA, "Non-FS Data" },
		{ 0xDB, "CP/M" },
		{ 0xDE, "Dell Utility" },
		{ 0xDF, "BootIt" },
		{ 0xE1, "DOS Access" },
		{ 0xE3, "DOS R/O" },
		{ 0xE4, "SpeedStor" },
		{ 0xEB, "BeOS" },
		{ 0xEE, "GPT" },
		{ 0xEF, "EFI FAT" },
		{ 0xF0, "Linux/PA-RISC Boot" },
		{ 0xF1, "SpeedStor" },
		{ 0xF2, "DOS secondary" },
		{ 0xF4, "SpeedStor" },
		{ 0xFB, "VMware VMFS" },
		{ 0xFC, "VMware VMKCORE" },
		{ 0xFD, "Linux RAID Auto" },
		{ 0xFE, "LANstep" },
		{ 0xFF, "XENIX BBT" }
	};
};