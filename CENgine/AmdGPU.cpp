#include "AmdGPU.h"

AmdGPU::AmdGPU(Device device)
	:
	GPU(device)
{ }

AmdGPU::~AmdGPU()
{

}

bool AmdGPU::Initialize()
{
	return true;
}

void AmdGPU::GetWorkload()
{

}