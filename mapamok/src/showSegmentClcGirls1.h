#pragma once

class showSegmentClcGirls1 : public showSegment
{
private:
    ofSoundPlayer snd;
	int nBands;
    ofxOnsetDetection onsetD;
	bool playing;

	effectMusicShader1* musicShader;

public:
	showSegmentClcGirls1();

	virtual void setup();
	virtual void start();
	virtual void end();
	virtual void update();
	virtual void render();

};