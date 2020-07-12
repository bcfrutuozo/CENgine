#include "ModelException.h"

#include <sstream>

ModelException::ModelException(const int line, const char* file, std::string note) noexcept
	:
	CENgineException(line, file),
	note(std::move(note))
{ }

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << CENgineException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "CENgine Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}