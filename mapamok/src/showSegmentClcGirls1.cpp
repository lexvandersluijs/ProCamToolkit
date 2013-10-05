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
	onsetD.setup();
    snd.loadSound("sound/1085.mp3");
    snd.setLoop(true);

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 600.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.addToggle("play", false);

	controlPanel.addSlider("pattern 1", 1, .1, 25);
	controlPanel.addSlider("pattern 2", 1, .1, 25);
	controlPanel.addSlider("light effect", 1, .1, 25);

	controlPanel.hide();
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
			//ofBackground(ofColor::red);
			currentColor = ofColor::red;
		}
		else 
		{
			currentColor = ofColor::black;
		}
	}
	else
		currentColor = ofColor::black;
}

void showSegmentClcGirls1::render()
{
	ofSetColor(currentColor);

	showSegment::render();
}