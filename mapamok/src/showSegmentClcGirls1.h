#pragma once

class showSegmentClcGirls1 : public showSegment
{

public:
	showSegmentClcGirls1();

	virtual void setup();
	virtual void start();
	virtual void end();
	virtual void update();
	virtual void render();

protected:
    ofSoundPlayer snd;
	int nBands;
    ofxOnsetDetection onsetD;
	bool playing;

	effectMusicShader1* musicShader;
	float segmentStartTime;

	trigger startTrigger;
	trigger startMelodyTrigger;
	trigger midSectionTrigger1;
	trigger midSectionTrigger2;
	trigger midSectionTrigger3;
	trigger midSectionTrigger4;
	trigger midSectionTrigger5;
	trigger startClappingTrigger;
	trigger stopClappingTrigger;
	trigger midOfEndSectionTrigger;
	trigger endSongTrigger;
	trigger heartFadeInTrigger;
	trigger heartFadeOutTrigger;

	fader centralPatternFader;
	fader lightsFader;

	bool overrideLightsFader;
};