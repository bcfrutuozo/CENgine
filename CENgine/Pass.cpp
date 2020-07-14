#include "Pass.h"
#include "RenderGraphCompileException.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Utilities.h"
#include "Sink.h"
#include "Source.h"

#include <sstream>

namespace RGP
{
	Pass::Pass(std::string name) noexcept
		:
		name(std::move(name))
	{ }

	Pass::~Pass()
	{ }

	void Pass::Reset() NOXND
	{ }

	const std::string& Pass::GetName() const noexcept
	{
		return name;
	}

	void Pass::Finalize()
	{
		for(auto& in : sinks)
		{
			in->PostLinkValidate();
		}
		for(auto& out : sources)
		{
			out->PostLinkValidate();
		}
	}

	const std::vector<std::unique_ptr<Sink>>& Pass::GetSinks() const
	{
		return sinks;
	}

	Source& Pass::GetSource(const std::string& name) const
	{
		for(auto& src : sources)
		{
			if(src->GetName() == name)
			{
				return *src;
			}
		}

		std::ostringstream oss;
		oss << "Ouput named [" << name << "] not found in pass: " << GetName();
		throw RGC_EXCEPTION(oss.str());
	}

	Sink& Pass::GetSink(const std::string& registeredName) const
	{
		for(auto& sk : sinks)
		{
			if(sk->GetRegisteredName() == registeredName)
			{
				return *sk;
			}
		}

		std::ostringstream oss;
		oss << "Input named [" << registeredName << "] not found in pass: " << GetName();
		throw RGC_EXCEPTION(oss.str());
	}

	void Pass::RegisterSink(std::unique_ptr<Sink> sink)
	{
		// Check for overlap of input names
		for(auto& sk : sinks)
		{
			if(sk->GetRegisteredName() == sink->GetRegisteredName())
			{
				throw RGC_EXCEPTION("Registered input overlaps with existing: " + sink->GetRegisteredName());
			}
		}

		sinks.push_back(std::move(sink));
	}

	void Pass::RegisterSource(std::unique_ptr<Source> source)
	{
		// check for overlap of output names
		for(auto& src : sources)
		{
			if(src->GetName() == source->GetName())
			{
				throw RGC_EXCEPTION("Registered output overlaps with existing: " + src->GetName());
			}
		}

		sources.push_back(std::move(source));
	}

	void Pass::SetSinkLinkage(const std::string& registeredName, const std::string& target)
	{
		auto& sink = GetSink(registeredName);
		auto targetSplit = SplitString(target, ".");
		if(targetSplit.size() != 2u)
		{
			throw RGC_EXCEPTION("Input target has incorrect format");
		}
		sink.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}
}