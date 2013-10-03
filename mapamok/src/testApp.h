#pragma once

#include "ofMain.h"
#include "ofxOnsetDetection.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "ofxThreadedVideo.h"
#include "LineArt.h"
#include "mainControlPanel.h"
#include "projector.h"
#include "meshView.h"
#include "modelView.h"
#include "projectorView.h"
#include "projectorConfiguration.h"
#include "showSegment.h"
#include "showDefinition.h"
#include "showSegmentDefault.h"
#include "showSegmentClcGirls1.h"

class testApp : public ofBaseApp {
public:
	testApp(projectorConfiguration& pconfig, showDefinition& s) : projConfig(pconfig), show(s)
	{
	}
	void setb(string name, bool value);
	void seti(string name, int value);
	void setf(string name, float value);
	bool getb(string name);
	int geti(string name);
	float getf(string name);
	
	void setup();
	void update();
	void draw();	
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	void drawViewportOutline(const ofRectangle & viewport);

	void reloadShaderIfNeeded();
	void setupControlPanel();
	void setupMesh();
	
	ofxAssimpModelLoader model;	
	ofVboMesh objectMesh;
	ofLight light;
	mainControlPanel panel;
	
	projectorConfiguration& projConfig;
	showDefinition& show;
	modelView selectionView;

	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;

};
