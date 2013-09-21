#pragma once

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

	void setName(const string& n)
	{
		name = n;
	}
	void saveCalibration();
	void loadCalibration();

private:
	string name;
	int windowWidth;
	int windowHeight;
	int mode; // OF_FULLSCREEN / OF_WINDOW
	projectorView* projViews;
	int nrOfProjViews;
	projectorView* viewToCalibrate; // NULL if none, otherwise ptr to view being calibrated

};