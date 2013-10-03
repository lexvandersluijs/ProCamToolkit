#pragma once

class projectorView;

class modelView : public meshView
{
private:
	

protected:

public:
	ofEasyCam cam;
	void draw(ofxControlPanel& panel, float mouseX, float mouseY, projectorView* projView, ofLight& light, ofShader& shader, ofTexture* texture); //ofImage& customPicture, ofxThreadedVideo& mappingMovie); 
	//void draw(ofxControlPanel& panel, float mouseX, float mouseY, projectorView* projView, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie); 
};