#include "Technique.h"
#include "Drawable.h"
#include "FrameGenerator.h"

void Technique::Submit(class FrameGenerator& frame,const class Drawable& drawable ) const noexcept
{
	if( isActive )
	{
		for( const auto& step : steps )
		{
			step.Submit( frame,drawable );
		}
	}
}

void Technique::InitializeParentReferences( const class Drawable & parent ) noexcept
{
	for( auto& s : steps )
	{
		s.InitializeParentReferences( parent );
	}
}