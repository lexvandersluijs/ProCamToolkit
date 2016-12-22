#pragma once

class showResource;

enum FadeState { Idle, FadingIn, FadingOut };

class effectSingleTextureMap : public effect
{
public:
	effectSingleTextureMap()
	{
		shaderPartialFilePath = "shaders/singletexture_fade";

		currentResource = NULL;
		nextResource = NULL;
		//currentTexture = NULL;

		fadeState = FadeState::Idle;
		fadeIn = false;
		fadeOut = false;
		fadeTime = 1.0f;
	}
	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);
	virtual void stop();

	void SetFadeParameters(bool fadeIn, bool fadeOut, float fadeTime);
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
	showResource* nextResource; // the one we want after the current fade-out is done
	//ofTexture* currentTexture;

	ofxAutoControlPanel* controlPanel;

	fader textureFader;
	bool fadeIn;
	bool fadeOut;
	FadeState fadeState;
	float fadeTime;
};

