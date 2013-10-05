#pragma once

class showResource;

class effectSingleTextureMap : public effect
{
public:
	effectSingleTextureMap()
	{
		currentResource = NULL;
		currentTexture = NULL;
	}
	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);

	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model);

	void updateSelectedResourceCheckbox(ofxAutoControlPanel& panel);
	showResource* findNewSelectedResource(ofxAutoControlPanel& panel, bool& anySelectionMade);
	void updateCurrentTexture(ofxAutoControlPanel& panel);
	ofTexture* getCurrentTexture();

protected:
	void drawModel(ofPolyRenderMode renderType, ofxAssimpModelLoader* model);

	showResource* currentResource;
	ofTexture* currentTexture;
};

