#include "ScriptParser.h"
#include "TexturePreprocessor.h"
#include "json.hpp"

#include <sstream>
#include <fstream>
#include <filesystem>

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
			for(const auto& j : top.at("commands"))
			{
				const auto command = j.at( "command" ).get<std::string>();
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
				else if(command == "publish")
				{
					Publish(params.at("dest"));
				}
				else
				{
					throw SCRIPT_ERROR("Unknown command: "s + command);
				}
			}
		}
	}
}

void ScriptParser::Publish(std::string path) const
{
	if(!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}

	// Copy executable
	std::filesystem::copy_file(R"(C:\Users\bcfru\source\repos\CENgine\bin\x64\Release\CENgine.exe)", path + R"(\CENgine.exe)", std::filesystem::copy_options::overwrite_existing);
	
	// Copy assimp ini
	std::filesystem::copy_file("imgui_default.ini", path + R"(\imgui_default.ini)", std::filesystem::copy_options::overwrite_existing);
	
	// Copy all dlls
	for(auto& p : std::filesystem::directory_iterator(R"(C:\Users\bcfru\source\repos\CENgine\bin\x64\Release)"))
	{
		if(p.path().extension() == L".dll")
		{
			std::filesystem::copy_file(p.path(), path + "\\" + p.path().filename().string(),
				std::filesystem::copy_options::overwrite_existing
			);
		}
	}
	
	// Copy compiled shaders
	std::filesystem::copy("Shaders", path + R"(\Shaders)", std::filesystem::copy_options::overwrite_existing);
	
	// Copy assets
	std::filesystem::copy("Images", path + R"(\Images)", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	std::filesystem::copy("Models", path + R"(\Models)", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	std::filesystem::copy("Fonts", path + R"(\Fonts)", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
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