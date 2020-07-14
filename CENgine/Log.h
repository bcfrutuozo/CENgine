#pragma once

#include "Timer.h"

#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

class Log
{
public:

	static void Start(const std::string& label = "") noexcept
	{
		Get_().Start_(label);
	}

	static void Mark(const std::string& label = "") noexcept
	{
		Get_().Mark_(label);
	}

private:

	struct Entry
	{
		Entry(std::string s, float t)
			:
			label(std::move(s)),
			time(t)
		{ }

		void WriteTo(std::ostream& out) const noexcept
		{
			using namespace std::string_literals;

			if(label.empty())
			{
				out << time * 1000.0f << "ms\n";
			}
			else
			{
				out << std::setw( 16 ) << std::left << "["s + label + "] "s << std::setw(6) << std::right << time * 1000.0f << "ms\n";
			}
		}

		std::string label;
		float time;
	};

	static Log& Get_() noexcept
	{
		static Log log;
		return log;
	}

	Log() noexcept
	{
		entries.reserve(3000);
	}

	~Log()
	{
		Flush_();
	}

	void Start_(const std::string& label = "") noexcept
	{
		entries.emplace_back(label, 0.0f);
	}

	void Mark_(const std::string& label = "") noexcept
	{
		float t = timer.Peek();
		entries.emplace_back(label, t);
	}

	void Flush_()
	{
		std::ofstream file("log.txt");
		file << std::setprecision(3) << std::fixed;
		for(const auto& e : entries)
		{
			e.WriteTo(file);
		}
	}

	Timer timer;
	std::vector<Entry> entries;
};

