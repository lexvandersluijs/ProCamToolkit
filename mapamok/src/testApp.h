#pragma once

#include "ofMain.h"
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

	void reloadShaderIfNeeded();
	void setupControlPanel();
	void setupMesh();
	void setupScreen_custom(float viewW, float viewH, float fov, float nearDist, float farDist);
	//void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	//void drawSelectionMode(projectorView* projView);
	//void drawRenderMode(projectorView* projView);
	//void render();
	//void drawModel(ofPolyRenderMode renderType, ofTexture* textureOverride);
    //void loadCalibration();
	//void saveCalibration();
	
	ofxAssimpModelLoader model;	
	ofVboMesh objectMesh;
	ofLight light;
	mainControlPanel panel;
	
	projectorConfiguration projConfig;
	modelView selectionView;

	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;

	// the movie we are going to project onto our shapes
	//ofVideoPlayer 		mappingMovie;
	ofxThreadedVideo		mappingMovie;

	// custom picture
	ofImage				customPicture0;
};
