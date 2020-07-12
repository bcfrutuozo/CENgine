#pragma once

// DRR - Dynamic Runtime Reflection 
namespace DRR
{
	class Buffer;
}

class TechniqueProbe
{
public:

	void SetTechnique(class Technique* pTech_in)
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}

	void SetStep(class Step* pStep_in)
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}

	bool VisitBuffer(class DRR::Buffer& buffer)
	{
		bufIdx++;
		return OnVisitBuffer(buffer);
	}
protected:

	virtual void OnSetTechnique()
	{ }

	virtual void OnSetStep()
	{ }

	virtual bool OnVisitBuffer(class DRR::Buffer&)
	{
		return false;
	}

	class Technique* pTech = nullptr;
	class Step* pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};
