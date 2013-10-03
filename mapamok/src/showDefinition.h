#pragma once

class showResource
{
public:
	string filePath;
	string name;

	virtual string makeGuiName() { return ""; }
	virtual bool isSelected(mainControlPanel& panel);
	virtual ofTexture* getTexturePtr() { return NULL; }
	virtual void comeIntoView() { }
	virtual void goOutOfView() { }
	virtual void update() { }
};

class movieResource : public showResource
{
public:
	movieResource()
	{
		movie = NULL;
	}
	~movieResource();

	void load();

	virtual string makeGuiName() { return "m: " + name; }
	virtual ofTexture* getTexturePtr();
	virtual void comeIntoView();
	virtual void goOutOfView();
	virtual void update();

private:
	ofxThreadedVideo*	movie;
};

class pictureResource : public showResource
{
public:

	pictureResource()
	{
		picture = NULL;
	}
	~pictureResource()
	{
		if(picture != NULL)
			delete picture;
	}
	void load();

	virtual string makeGuiName() { return "p: " + name; }
	virtual ofTexture* getTexturePtr();
	virtual void comeIntoView() { }
	virtual void goOutOfView() { }
	virtual void update() { }

private:
	ofImage*				picture;
};

class showDefinition
{
public:
	showDefinition()
	{
		currentResource = NULL;
	}
	~showDefinition();

	void load(string configName);

	void addMovie(string filePath, string name);
	void addPicture(string filePath, string name);

	void loadResources();

	void updateSelectedResourceCheckbox(mainControlPanel& panel);
	showResource* findNewSelectedResource(mainControlPanel& panel, bool& anySelectionMade);
	void updateCurrentTexture(mainControlPanel& panel);
	ofTexture* getCurrentTexture();

	std::vector<movieResource*> movies;
	std::vector<pictureResource*> pictures;

private:
	showResource* currentResource;
};
