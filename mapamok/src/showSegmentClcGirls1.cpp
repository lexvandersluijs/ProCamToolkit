#include "testApp.h"

void showSegmentClcGirls1::setup()
{
	name = "Segment 1";

	// --- onset detection ---
	nBands = 1024;
	onsetD.setup();
    snd.loadSound("sound/1085.mp3");
    snd.setLoop(true);

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 600.0f);
	controlPanel.addPanel("Interaction");

	controlPanel.addSlider("pattern 1", 1, .1, 25);
	controlPanel.addSlider("pattern 2", 1, .1, 25);
	controlPanel.addSlider("light effect", 1, .1, 25);
}

void showSegmentClcGirls1::start()
{
    snd.play(); 
}

void showSegmentClcGirls1::update()
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

void showSegmentClcGirls1::draw()
{
	ofSetColor(currentColor);
}