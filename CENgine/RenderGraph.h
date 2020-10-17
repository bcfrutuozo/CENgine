#pragma once

#include "Conditional_noexcept.h"

#include <string>
#include <vector>
#include <memory>

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace RGP
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:

		RenderGraph(Graphics& graphics);
		~RenderGraph();

		void Execute(Graphics& graphics) NOXND;
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue(const std::string& passName);
		void StoreDepth(Graphics& graphics, const std::string& path);
	protected:

		void SetSinkTarget(const std::string& sinkName, const std::string& target);
		void AddGlobalSource(std::unique_ptr<Source> source);
		void AddGlobalSink(std::unique_ptr<Sink> sink);
		void AppendPass(std::unique_ptr<Pass> pass);
		void Finalize();
		Pass& FindPassByName(const std::string& name);

		std::shared_ptr<Bind::RenderTarget> backBufferTarget;
		std::shared_ptr<Bind::DepthStencil> masterDepth;
	private:

		void LinkSinks(Pass& pass);
		void LinkGlobalSinks();

		std::vector<std::unique_ptr<Pass>> passes;
		std::vector<std::unique_ptr<Source>> globalSources;
		std::vector<std::unique_ptr<Sink>> globalSinks;
		bool finalized = false;
	};
}