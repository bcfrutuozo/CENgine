#pragma once

#include "CENgineException.h"

#include <vector>
#include <string>

class ScriptParser
{
public:

	class Exception : public CENgineException
	{
	public:

		Exception(int line, const char* file, const std::string& script = "", const std::string& message = "") noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
	private:

		std::string script;
		std::string message;
	};

	ScriptParser(const std::vector<std::string>& args);
private:

	void Publish(std::string path) const;
};