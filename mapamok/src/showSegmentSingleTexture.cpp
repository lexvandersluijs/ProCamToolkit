#include "testApp.h"

showSegmentSingleTexture::showSegmentSingleTexture()
{
	name = "Texture Map";
}
void showSegmentSingleTexture::setup()
{
	effects.push_back(new effectSingleTextureMap());

	controlPanel.setup(name, 850.0f, 5.0f, 280.0f, 600.0f);
	controlPanel.addPanel("Interaction");

	allEffectsSetupControlPanels();
}