#include "PassOutput.h"
#include "RenderGraphCompileException.h"

#include <algorithm>
#include <cctype>

const std::string& PassOutput::GetName() const noexcept
{
	return name;
}

std::shared_ptr<Bind::Bindable> PassOutput::YieldImmutable()
{
	throw RGC_EXCEPTION( "Output cannot be accessed as immutable" );
}

std::shared_ptr<Bind::BufferResource> PassOutput::YieldBuffer()
{
	throw RGC_EXCEPTION( "Output cannot be accessed as buffer" );
}

PassOutput::PassOutput(std::string name_in)
:
	name( std::move( name_in ) )
{
	if( name.empty() )
	{
		throw RGC_EXCEPTION( "Empty output name" );
	}

	const bool nameCharsValid = std::all_of( name.begin(),name.end(),[]( char c ) {
		return std::isalnum( c ) || c == '_';
	} );

	if( !nameCharsValid || std::isdigit( name.front() ) )
	{
		throw RGC_EXCEPTION( "Invalid output name: " + name );
	}
}