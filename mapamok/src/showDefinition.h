#pragma once

class showResource
{
public:
	string filePath;
	string name;

	virtual string makeGuiName() { return ""; }
	virtual bool isSelected(ofxControlPanel& panel);
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
	virtual ofImage* getImagePtr() { return picture; }
	virtual void comeIntoView() { }
	virtual void goOutOfView() { }
	virtual void update() { }

private:
	ofImage*				picture;
};

class showDefinition
{
public:
	showDefinition();
	~showDefinition();

	void load(string configName);

	void setup(); // let each segment initialize itself, creating a control panel if needed
	void update(mainControlPanel& panel);

	void addMovie(string filePath, string name);
	void addPicture(string filePath, string name);
	pictureResource* findPictureByName(string name);

	std::vector<movieResource*> movies;
	std::vector<pictureResource*> pictures;

	std::vector<showSegment*> showSegments;
	showSegment* currentSegment;
	int currentSegmentIndex;

	void setCurrentSegment(int segmentIndex);

	static showDefinition* getInstance();

private:
	static showDefinition* instance;

	void loadResources();

};
