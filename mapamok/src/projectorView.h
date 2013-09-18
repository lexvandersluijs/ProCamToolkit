#pragma once

class projectorConfiguration;

class projectorView : public meshView
{
public:
	string name;
	projector proj;
	int index;
	

	// each projector has a pointer to the total
	// configuration so that it can check if it is the one being calibrated
	projectorConfiguration* projConfiguration;

	projectorView()
	{
		projConfiguration = NULL;
	}
	void setConfiguration(projectorConfiguration* projConfig)
	{
		projConfiguration = projConfig;
	}
	void setModelAndMesh(ofxAssimpModelLoader* mdl, ofMesh* m)
	{
		meshView::setModelAndMesh(mdl, m);

		if(objectMesh != NULL)
			proj.initializeFromMesh(*objectMesh);
	}
	void updateCalibration(mainControlPanel& panel) 
	{
		// LS: we should have these properties for each projector individually. 
		// for now: assume equal brand/model projectors
		float aov = panel.getValueF("aov");

		// generate flags
		#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
		int flags =
			CV_CALIB_USE_INTRINSIC_GUESS |
			getFlag(CV_CALIB_FIX_PRINCIPAL_POINT) |
			getFlag(CV_CALIB_FIX_ASPECT_RATIO) |
			getFlag(CV_CALIB_FIX_K1) |
			getFlag(CV_CALIB_FIX_K2) |
			getFlag(CV_CALIB_FIX_K3) |
			getFlag(CV_CALIB_ZERO_TANGENT_DIST);

		proj.updateCalibration(aov, flags, viewport);
	}

	virtual void draw(ofxControlPanel& panel, float mouseX, float mouseY, ofLight& light, ofShader& shader, ofImage& customPicture, ofxThreadedVideo& mappingMovie);
	//virtual void draw(ofxControlPanel& panel, float mouseX, float mouseY, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie);
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