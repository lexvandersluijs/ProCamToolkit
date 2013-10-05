#include "testApp.h"

void showSegment::update()
{
	for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
	{
		(*eit)->update(controlPanel);
	}
}

void showSegment::render()
{
	for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
	{
		(*eit)->render(controlPanel, model);
	}
}