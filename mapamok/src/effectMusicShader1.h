#pragma once

class pictureResource;

class effectMusicShader1 : public effect
{
public:
	effectMusicShader1();

	virtual void setup();
	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);

	void pulse(int index);
	void setAvgFreqLevel(float avg, float avgLow, float avgMid, float avgHigh);


protected:
	float pulseTimes[5];
	float pulseFactors[5];
	int nrOfPulseTimes;

	ofVec3f initialLightPositions[3];
	ofVec3f initialLightColors[3];

	pictureResource* picture1;
	pictureResource* stars;

	int prevLightStepIndex;
	float lightBarLowY;
	float lightBarHighY;

	ofxControlPanel* controlPanel;  // hmm.. shouldn't all effects have pointer to their control panel (instead of passing it in from the segment all the time?)
};

