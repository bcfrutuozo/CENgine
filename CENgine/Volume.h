#pragma once

#include "Peripheral.h"

class Volume : public Peripheral
{
public:

	Volume(Device p_Device);
	~Volume();

	void Initialize() override;
	void GetWorkload() override;
	void ShowWidget() override;
private:
};

