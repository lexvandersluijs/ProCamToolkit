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

protected:
	float pulseFactors[1];
	int nrOfPulseFactors;

	ofVec3f initialLightPositions[3];
	ofVec3f initialLightColors[3];

	pictureResource* picture1;
	pictureResource* stars;
};

