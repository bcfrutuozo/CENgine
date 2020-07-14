#pragma once

#include "Conditional_noexcept.h"

#include <vector>
#include <array>
#include <string>
#include <memory>

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace RGP
{
	class Sink;
	class Source;

	class Pass
	{
	public:

		Pass(std::string name) noexcept;
		virtual ~Pass();
		virtual void Execute(Graphics& graphics) const NOXND = 0;
		virtual void Reset() NOXND;
		const std::string& GetName() const noexcept;
		const std::vector<std::unique_ptr<Sink>>& GetSinks() const;
		Source& GetSource(const std::string& registeredName) const;
		Sink& GetSink(const std::string& registeredName) const;
		void SetSinkLinkage(const std::string& registeredName, const std::string& target);
		virtual void Finalize();
	protected:

		void RegisterSink(std::unique_ptr<Sink> sink);
		void RegisterSource(std::unique_ptr<Source> source);
	private:

		std::vector<std::unique_ptr<Sink>> sinks;
		std::vector<std::unique_ptr<Source>> sources;
		std::string name;
	};
}