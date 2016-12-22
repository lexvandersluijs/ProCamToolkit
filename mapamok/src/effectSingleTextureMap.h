#pragma once

class showResource;

enum FadeState { Idle, FadingIn, FadingOut };

class effectSingleTextureMap : public effect
{
public:
	effectSingleTextureMap()
	{
		currentResource = NULL;
		currentTexture = NULL;

		fadeState = FadeState::Idle;
		fadeIn = false;
		fadeOut = false;
		fadeTime = 1.0f;
	}
	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);
	virtual void stop();

	void SwitchState(FadeState newState);

	void updateSelectedResourceCheckbox(ofxAutoControlPanel& panel);
	showResource* findNewSelectedResource(ofxAutoControlPanel& panel, bool& anySelectionMade);
	void updateCurrentTexture(ofxAutoControlPanel& panel);
	ofTexture* getCurrentTexture();

	void addSelectedResourceName(const std::string& name)
	{
		selectedResourceNames.push_back(name);
	}

protected:
	void drawModel(ofPolyRenderMode renderType, ofxAssimpModelLoader* model);

	// list of names of resources in showDefinition (which functions as a resource manager)
	std::vector<std::string> selectedResourceNames;

	showResource* currentResource;
	ofTexture* currentTexture;

	ofxAutoControlPanel* controlPanel;

	bool fadeIn;
	bool fadeOut;
	FadeState fadeState;
	float fadeTime;
};

