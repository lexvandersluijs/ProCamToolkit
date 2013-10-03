#pragma once

class showSegmentClcGirls1 : public showSegment
{
private:
	ofShader shader;

    ofSoundPlayer snd;
	int nBands;
    ofxOnsetDetection onsetD;

	ofColor currentColor;

	ofxAutoControlPanel controlPanel;

public:
	virtual void setup();
	virtual void start();
	virtual void update();
	virtual void draw();

};