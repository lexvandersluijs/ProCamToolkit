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
	trigger startClappingTrigger;
	trigger stopClappingTrigger;
	trigger endSongTrigger;

	fader centralPatternFader;
	fader lightsFader;

};