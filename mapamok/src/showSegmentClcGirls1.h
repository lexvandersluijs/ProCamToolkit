#pragma once

class showSegmentClcGirls1 : public showSegment
{
private:
	ofShader shader;

    ofSoundPlayer snd;
	int nBands;
    ofxOnsetDetection onsetD;

	ofColor currentColor;

public:
	showSegmentClcGirls1();

	virtual void setup();
	virtual void start();
	virtual void update();
	virtual void render();

};