#include "testApp.h"

showSegmentDefault::showSegmentDefault()
{
	name = "Default";
}

void showSegmentDefault::setup()
{
	effects.push_back(new effectDefaultShader());

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 800.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.hide();

	allEffectsSetupControlPanels();
}

void showSegmentDefault::update()
{
	// default implementation: all effects get new input from control panel of this segment
	showSegment::update();

}

void showSegmentDefault::render()
{
	showSegment::render();
}