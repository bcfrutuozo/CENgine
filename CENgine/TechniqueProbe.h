#pragma once

#include <limits>

// DRR - Dynamic Runtime Reflection 
namespace DRR
{
	class Buffer;
}

class Technique;
class Step;

class TechniqueProbe
{
public:

	virtual ~TechniqueProbe() { }

	void SetTechnique(Technique* pTech_in)
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}

	void SetStep(Step* pStep_in)
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}

	bool VisitBuffer(DRR::Buffer& buffer)
	{
		bufIdx++;
		return OnVisitBuffer(buffer);
	}
protected:

	virtual void OnSetTechnique()
	{ }

	virtual void OnSetStep()
	{ }

	virtual bool OnVisitBuffer(DRR::Buffer&)
	{
		return false;
	}

	Technique* pTech = nullptr;
	Step* pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};
