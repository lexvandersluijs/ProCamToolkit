#include "testApp.h"

showSegmentSingleTexture::showSegmentSingleTexture()
{
	name = "Texture Map";
}
void showSegmentSingleTexture::setup()
{
	effects.push_back(new effectSingleTextureMap());

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 600.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.hide();

	allEffectsSetupControlPanels();
}