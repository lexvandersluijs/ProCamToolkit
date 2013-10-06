#pragma once

class effectMusicShader1 : public effect
{
public:
	effectMusicShader1();

	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);

	void pulse(int index);

protected:
	float pulseFactors[1];
	int nrOfPulseFactors;
};

