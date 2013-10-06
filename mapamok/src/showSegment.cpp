#include "testApp.h"

void showSegment::setup()
{
	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 800.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.hide();


	for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
	{
		(*eit)->setup();
	}

	allEffectsSetupControlPanels();
}

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
		(*eit)->render(controlPanel, model, mesh);
	}
}


void showSegment::start()
{
	controlPanel.show();
}

void showSegment::end()
{ 
	controlPanel.hide();

	for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
	{
		(*eit)->stop();
	}
}
