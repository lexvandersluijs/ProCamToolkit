#include "testApp.h"

showSegmentClcGirls1::showSegmentClcGirls1()
{
	name = "CLC Girls 1";
	playing = false;
}

void showSegmentClcGirls1::setup()
{
	// --- onset detection ---
	nBands = 1024;
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX);
    snd.loadSound("sound/NeedYourLove-Gold_CLCMix_Joel_LQ.mp3");
    snd.setLoop(true);

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 600.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.addToggle("play", false);

	controlPanel.addSlider("pattern 1", 1, .1, 25);
	controlPanel.addSlider("pattern 2", 1, .1, 25);
	controlPanel.addSlider("light effect", 1, .1, 25);

	controlPanel.hide();

	// after the segment has added its controls, also allow the effects underneath it to add its GUI elements
	allEffectsSetupControlPanels();

	// get a pointer to the music shader for easy access
	musicShader = static_cast<effectMusicShader1*>(effects[0]);
}

void showSegmentClcGirls1::start()
{
	showSegment::start();
}

void showSegmentClcGirls1::end()
{
	showSegment::end();

    snd.stop(); 
	playing = false;
}

void showSegmentClcGirls1::update()
{
	// all effects should update too
	showSegment::update();

	if(controlPanel.getValueB("play") && playing == false)
	{
		snd.play(); 
		playing = true;
	}
	else 
		if (controlPanel.getValueB("play") == false && snd.getIsPlaying() == true)
		{
			snd.stop();
			playing = false;
		}

	if(snd.getIsPlaying())
	{
		if (onsetD.isOnsetting(ofSoundGetSpectrum(nBands)))
		{
			musicShader->pulse(0);
		}
	}
}

void showSegmentClcGirls1::render()
{

	// temporary: probably we do not simply want all effects to render after each other.. need to set blend 
	// states, clear depth buffer, etc
	showSegment::render();
}