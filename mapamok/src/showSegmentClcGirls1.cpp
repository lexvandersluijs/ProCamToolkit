#include "testApp.h"

showSegmentClcGirls1::showSegmentClcGirls1()
{
	name = "CLC Girls 1";
	playing = false;
}

void showSegmentClcGirls1::setup()
{
	// let all effects set themselves up
	for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
	{
		(*eit)->setup();
	}

	// --- onset detection ---
	nBands = 1024;
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX);
    snd.loadSound("sound/NextStep_CLCEdit_Richard.mp3");
    snd.setLoop(false);

	controlPanel.setup(name, 285.0f, 5.0f, 280.0f, 600.0f, false);
	controlPanel.addPanel("Interaction");
	controlPanel.addToggle("play", false);

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
		float* spectrum = ofSoundGetSpectrum(nBands);

		float avg = 0;
		float avgLow = 0;
		float avgMid = 0;
		float avgHigh = 0;
		int bandwidth = nBands/3;
		int bandwidth2 = bandwidth*2;
		for(int i=0; i<nBands; ++i)
		{
			if(i<bandwidth)
			{
				avgLow += spectrum[i];
			}
			else if(i>bandwidth && i<bandwidth2)
			{
				avgMid += spectrum[i];
			}
			else
			{
				avgHigh += spectrum[i];
			}
			avg += spectrum[i];
		}

		avg /= (float)nBands;
		avgLow /= (float)bandwidth;
		avgMid /= (float)bandwidth;
		avgHigh /= (float)bandwidth;

		float normalizationFactor = 10.f; // let's try to get these in the 0 to 1 range..
		avg *= normalizationFactor;
		avgLow *= normalizationFactor;
		avgMid *= normalizationFactor*3.f;
		avgHigh *= normalizationFactor*10.f;

		musicShader->setAvgFreqLevel(avg, avgLow, avgMid, avgHigh);

		if (onsetD.isOnsetting(spectrum))
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