#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "LineArt.h"
#include "projector.h"
#include "mainControlPanel.h"

class testApp : public ofBaseApp {
public:
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

	void setupControlPanel();
	void setupMesh();
	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
	void drawModel(ofPolyRenderMode renderType);
    void loadCalibration();
	void saveCalibration();
	
	ofxAssimpModelLoader model;	
	ofEasyCam cam;
	ofVboMesh objectMesh;
	ofMesh imageMesh;
	ofLight light;
	mainControlPanel panel;
	
	ofRectangle projectorViewport;
	projector proj;
	
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;

	// the movie we are going to project onto our shapes
	ofVideoPlayer 		fingerMovie;

	// custom picture
	ofImage				customPicture0;
};
