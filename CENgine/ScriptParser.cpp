#include "ScriptParser.h"
#include "TexturePreprocessor.h"
#include "json.hpp"

#include <sstream>
#include <fstream>

using namespace std::string_literals;

#define SCRIPT_ERROR(msg) Exception(__LINE__, __FILE__, path, (msg));

ScriptParser::ScriptParser(const std::vector<std::string>& args)
{
	if(args.size() >= 2 && args[0] == "--commands")
	{
		const auto path = args[1];
		std::ifstream script(path);

		if(!script.is_open())
		{
			throw SCRIPT_ERROR("Unable to open script file"s);
		}

		jsonlib::json top;
		script >> top;

		if(top.at("enabled"))
		{
			for(const auto& j : top)
			{
				const auto command = j.at("command").get<std::string>();
				const auto params = j.at("params");
				if(command == "flip-y")
				{
					const auto source = params.at("source");
					TexturePreprocessor::FlipYNormalMap(source, params.value("dest", source));
				}
				else if(command == "flip-y-obj")
				{
					TexturePreprocessor::FlipYAllNormalMapsInObject(params.at("source"));
				}
				else if(command == "validate-nmap")
				{
					TexturePreprocessor::ValidateNormalMap(params.at("source"), params.at("min"), params.at("max"));
				}
				else if(command == "make-stripes")
				{
					TexturePreprocessor::MakeStripes(params.at("dest"), params.at("size"), params.at("stripeWidth"));
				}
				else
				{
					throw SCRIPT_ERROR("Unknown command: "s + command);
				}
			}
		}
	}
}

ScriptParser::Exception::Exception(int line, const char* file, const std::string& script, const std::string& message) noexcept
	:
	CENgineException(-1, "@ScriptParserAbort"),
	script(script),
	message(message)
{ }

const char* ScriptParser::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Script File]" << script << std::endl
		<< message;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ScriptParser::Exception::GetType() const noexcept
{
	return "Script Parser Error";
}