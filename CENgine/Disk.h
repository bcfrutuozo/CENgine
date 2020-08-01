#pragma once

#include "Peripheral.h"

class Disk : public Peripheral
{
public:

	Disk(Device driver);
	~Disk();
	void Initialize() override { }
	void ShowWidget() override { }
	void GetWorkload() override { }
};