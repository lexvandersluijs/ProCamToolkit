#include "testApp.h"

showSegmentClcGirls1::showSegmentClcGirls1()
{
	name = "CLC Girls 1";
	playing = false;
	segmentStartTime = 0;
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


	startTrigger.setup(0.f);
	startMelodyTrigger.setup(30.f);
	midSectionTrigger1.setup(38.f);
	midSectionTrigger2.setup(46.f);
	midSectionTrigger3.setup(54.f);
	midSectionTrigger4.setup(62.f);
	midSectionTrigger5.setup(70.f);
	startClappingTrigger.setup(100.f);
	stopClappingTrigger.setup(115.f);
	midOfEndSectionTrigger.setup(132.f);
	endSongTrigger.setup(156.f);
	heartFadeInTrigger.setup(160.f);
	heartFadeOutTrigger.setup(161.5f);

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


	float t = ofGetElapsedTimef();

	// ---------- check if we should stop / start the segment based on GUI ------
	if(controlPanel.getValueB("play") && playing == false)
	{
		snd.play(); 
		segmentStartTime = t;
		playing = true;
	}
	else 
	{
		if (controlPanel.getValueB("play") == false && snd.getIsPlaying() == true)
		{
			snd.stop();
			playing = false;
		}
	}

	if(playing)
	{
		// -------------- update faders and change high-level settings -------------
		float timeInSegment = t - segmentStartTime;
		if(startTrigger.evaluate(timeInSegment))
			lightsFader.fadeIn();

		if(startMelodyTrigger.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(1); // thumping heart
			musicShader->setCentralPatternMotion(1);
			centralPatternFader.fadeIn();
		}

		if(midSectionTrigger1.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(0); // rotating flowers
			musicShader->setLightingMode(1); // bars moving upward
			musicShader->setCentralPatternMotion(2);
		}
		if(midSectionTrigger2.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(0); // rotating flowers
			musicShader->setCentralPatternMotion(2);
			musicShader->setLightingMode(0); // stars
		}
		if(midSectionTrigger3.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(1); // heart
			musicShader->setCentralPatternMotion(1); // thumping
			musicShader->setLightingMode(1); // bars moving upward
		}
		if(midSectionTrigger4.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(0); // flowers
			musicShader->setCentralPatternMotion(2); // rotating
			musicShader->setLightingMode(0); // stars
		}
		if(midSectionTrigger5.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(1); // heart
			musicShader->setCentralPatternMotion(1); // thumping
			musicShader->setLightingMode(0); // stars
		}

		// 1:40 -> clapping section, 15 seconds
		if(startClappingTrigger.evaluate(timeInSegment))
		{
			lightsFader.fadeOut();
			centralPatternFader.fadeOut();
			musicShader->setLightingMode(2); // to light bars mode
		}

		if(stopClappingTrigger.evaluate(timeInSegment))
		{
			lightsFader.fadeIn();
			centralPatternFader.fadeIn();
			musicShader->setLightingMode(0); // back to stars
		}
		if(midOfEndSectionTrigger.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(0); // flowers
			musicShader->setCentralPatternMotion(2);
			musicShader->setLightingMode(0); // back to stars
		}

		if(endSongTrigger.evaluate(timeInSegment))
		{
			lightsFader.fadeOut();
			centralPatternFader.fadeOut();
		}

		if(heartFadeInTrigger.evaluate(timeInSegment))
		{
			musicShader->setCentralPattern(1);
			musicShader->setCentralPatternMotion(0); // no motion
			centralPatternFader.fadeIn();
		}
		if(heartFadeOutTrigger.evaluate(timeInSegment))
		{
			centralPatternFader.fadeOut();
		}
		centralPatternFader.update();
		lightsFader.update();

		musicShader->setMainLightsLevel(lightsFader.getCurrentValue());
		musicShader->setCentralPatternLevel(centralPatternFader.getCurrentValue());

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
}

void showSegmentClcGirls1::render()
{

	// temporary: probably we do not simply want all effects to render after each other.. need to set blend 
	// states, clear depth buffer, etc
	showSegment::render();
}