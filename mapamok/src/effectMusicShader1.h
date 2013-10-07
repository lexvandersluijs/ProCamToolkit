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

	void setMainLightsLevel(float level)
	{
		mainLightsLevel = level;
	}
	void setCentralPatternLevel(float level);
	void setLightingMode(int mode);
	void setCentralPattern(int patternIndex);
	void setCentralPatternMotion(int motionIndex); // 0=none, 1=thumping, 2=rotating

protected:
	float pulseTimes[5];
	float pulseFactors[5];
	int nrOfPulseTimes;

	int lightingMode;
	int centralPatternIndex;
	int centralPatternMotion;

	ofVec3f initialLightPositions[3];
	ofVec3f initialLightColors[3];

	pictureResource* picture1;
	pictureResource* heart;
	pictureResource* stars;

	int prevLightStepIndex;
	float lightBarLowY;
	float lightBarHighY;

	float mainLightsLevel;
	float centralPatternLevel;

	ofxControlPanel* controlPanel;  // hmm.. shouldn't all effects have pointer to their control panel (instead of passing it in from the segment all the time?)
};

