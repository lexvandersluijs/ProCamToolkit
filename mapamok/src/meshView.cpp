#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void meshView::drawLabeledPoint(int label, ofVec2f position,  int pointSize, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);

	// LS: to make compatible with multi-viewport operation, we use the viewport's width
	// and height, instead of the original ofGetWidth() etc to get the screen's dimensions
	//GLint vp[4];
	//glGetIntegerv(GL_VIEWPORT, vp);
	float w = viewport.width; //(float)vp[2];
	float h = viewport.height; //(float)vp[3];

	ofSetLineWidth(1.5);
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, pointSize);
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}

