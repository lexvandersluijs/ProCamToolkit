#pragma once

class projectorConfiguration;

class projectorView
{
public:
	projector proj;
	ofRectangle viewport;
	bool setupMode;
	int index;
	ofEasyCam cam;

	// each projector has a pointer to the total
	// configuration so that it can check if it is the one being calibrated
	projectorConfiguration* projConfiguration;

	projectorView()
	{
		setupMode = false;
		projConfiguration = NULL;
	}
	void setConfiguration(projectorConfiguration* projConfig)
	{
		projConfiguration = projConfig;
	}
};

class projectorConfiguration
{
public:

	projectorConfiguration()
	{
		projViews = NULL;
		nrOfProjViews = 0;
	}
	projectorConfiguration(int nrOfViews)
	{
		initialize(nrOfViews);
	}
	~projectorConfiguration()
	{
		delete [] projViews;
	}
	void initialize(int nrOfViews)
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

private:
	projectorView* projViews;
	int nrOfProjViews;
	projectorView* viewToCalibrate; // NULL if none, otherwise ptr to view being calibrated

};