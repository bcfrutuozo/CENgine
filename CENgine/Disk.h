#pragma once

#include "Peripheral.h"

class Disk : public Peripheral
{
public:

	Disk(Device driver);
	void Initialize() override { }
	void Shutdown() override { }
	const long GetWorkload() override { return 0; }
};