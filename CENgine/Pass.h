#pragma once

#include "Conditional_noexcept.h"
#include "PassInput.h"
#include "PassOutput.h"

#include <vector>
#include <array>
#include <string>

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

class Pass
{
public:

	Pass(std::string name) noexcept;
	virtual void Execute(Graphics& graphics) const NOXND = 0;
	virtual void Reset() NOXND;
	const std::string& GetName() const noexcept;
	const std::vector<std::unique_ptr<PassInput>>& GetInputs() const;
	PassOutput& GetOutput(const std::string& registeredName) const;
	PassInput& GetInput(const std::string& registeredName) const;
	void SetInputSource(const std::string& registeredName, const std::string& target);
	virtual void Finalize();
	virtual ~Pass() = default;
protected:

	void RegisterInput(std::unique_ptr<PassInput> input);
	void RegisterOutput(std::unique_ptr<PassOutput> output);
	void BindBufferResources(Graphics& graphics) const NOXND;
	
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
private:

	std::vector<std::unique_ptr<PassInput>> inputs;
	std::vector<std::unique_ptr<PassOutput>> outputs;
	std::string name;
};

