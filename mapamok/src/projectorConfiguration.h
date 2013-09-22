#pragma once

class projectorConfiguration
{
public:

	projectorConfiguration()
	{
		name = "new";
		primaryScreenWidth = 0;
		primaryScreenHeight = 0;
		virtualScreenWidth = 0;
		virtualScreenHeight = 0;
		windowWidth = 0;
		windowHeight = 0;

		projViews = NULL;
		nrOfProjViews = 0;
	}

	~projectorConfiguration()
	{
		delete [] projViews;
	}
	void initialize(int primScreenWidth, int primScreenHeight, int virtScreenWidth, int virtScreenHeight)
	{
		primaryScreenWidth = primScreenWidth;
		primaryScreenHeight = primScreenHeight;
		virtualScreenWidth = virtScreenWidth;
		virtualScreenHeight = virtualScreenHeight;
	}
	int getPrimaryScreenWidth()
	{
		return primaryScreenWidth;
	}
	int getPrimaryScreenHeight()
	{
		return primaryScreenHeight;
	}
	void allocateViewports(int nrOfViews)
	{
		nrOfProjViews = nrOfViews;
		projViews = new projectorView[nrOfProjViews];
		for(int i=0; i<nrOfProjViews; i++)
			projViews[i].setConfiguration(this);
	}
	int numProjectorViews()
	{
		return nrOfProjViews;
	}
	void setViewToCalibrate(int index)
	{
		viewToCalibrate = &projViews[index];
	}
	void setViewToCalibrate(projectorView* projView)
	{
		viewToCalibrate = projView;
	}
	projectorView& getProjView(int index)
	{
		// if index in range etc
		return projViews[index];
	}
	projectorView* getProjViewPtr(int index)
	{
		// if index in range etc
		return &projViews[index];
	}
	projectorView* getViewToCalibrate()
	{
		return viewToCalibrate;
	}

	void setName(const string& n)
	{
		name = n;
	}
	void setWindowWidth(int w)
	{
		windowWidth = w;
	}
	void setWindowHeight(int h)
	{
		windowHeight = h;
	}
	int getWindowWidth()
	{
		return windowWidth;
	}
	int getWindowHeight()
	{
		return windowHeight;
	}
	void loadConfiguration(string name);

	void saveCalibration();
	void loadCalibration();

private:
	string name;
	int primaryScreenWidth;
	int primaryScreenHeight;
	int virtualScreenWidth;
	int virtualScreenHeight;
	int windowWidth;
	int windowHeight;
	int mode; // OF_FULLSCREEN / OF_WINDOW
	projectorView* projViews;
	int nrOfProjViews;
	projectorView* viewToCalibrate; // NULL if none, otherwise ptr to view being calibrated

};