#pragma once

class meshView
{
protected:
	ofRectangle viewport;
	ofxAssimpModelLoader* model;	
	ofMesh* objectMesh;
	ofMesh imageMesh;

public:
	virtual void draw(ofxControlPanel& panel)
	{}

	virtual void setModelAndMesh(ofxAssimpModelLoader* mdl, ofMesh* m)
	{
		model = mdl;
		objectMesh = m;
	}
	ofRectangle getViewport()
	{
		return viewport;
	}
	void setViewport(float x, float y, float width, float height)
	{
		viewport.x = x;
		viewport.y = y;
		viewport.width = width;
		viewport.height = height;
	}
	bool screenToViewport(float mouseX, float mouseY, float& vpX, float& vpY)
	{
		// check if mouse is inside viewport, in screen space
		if((mouseX >= viewport.x) && (mouseX < (viewport.x + viewport.width)) &&
			(mouseY >= viewport.y) && (mouseY < (viewport.y + viewport.height)))
		{
			vpX = mouseX - viewport.x;
			vpY = mouseY - viewport.y; 

			return true;
		}
		return false;
	}


	void drawLabeledPoint(int label, ofVec2f position,  int pointSize, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);

	// LvdS: adapted from ofxAssimpModelLoader
	//void drawModel(ofPolyRenderMode renderType, ofTexture* textureOverride);

	//void render(ofxControlPanel& panel, ofShader& shader, ofTexture* texture); //ofImage& customPicture, ofxThreadedVideo& mappingMovie);
	//void render(ofxControlPanel& panel, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie);
	
};