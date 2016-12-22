#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void projectorView::draw(ofxControlPanel& panel, float mouseX, float mouseY, showSegment* currentShowSegment) //ofImage& customPicture, ofxThreadedVideo& mappingMovie)
//void projectorView::draw(ofxControlPanel& panel, float mouseX, float mouseY, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie)
{
	if(proj.calibrationReady) {

		ofPushView();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
 		proj.intrinsics.loadProjectionMatrix_dontSetViewport(1, 10000);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		applyMatrix(proj.modelMatrix);
		
		currentShowSegment->render(); //customPicture, mappingMovie);

		if(panel.getValueI("mode") == 0) {
			imageMesh = getProjectedInViewportMesh(*objectMesh);	
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		ofPopView();
	}

	
	if(panel.getValueI("mode") == 0) {

		float vpX;
		float vpY;
		
		int pointSize = panel.getValueI("selectedPointSize");

		// draw all reference points cyan
		int n = proj.referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(proj.referencePoints[i]) {
				drawLabeledPoint(i, toOf(proj.imagePoints[i]), pointSize, cyanPrint);
			}
		}
		
		if(screenToViewport(mouseX, mouseY, vpX, vpY))
		{
			// move points that need to be dragged
			// draw selected yellow
			int choice = panel.getValueI("selectionChoice");
			if(panel.getValueB("selected")) {
				proj.referencePoints[choice] = true;	
				Point2f& cur = proj.imagePoints[choice];	
				if(cur == Point2f()) {
					if(proj.calibrationReady) {
						cur = toCv(ofVec2f(imageMesh.getVertex(choice)));
					} else {
						cur = Point2f(vpX, vpY);
					}
				}
			}
			if(panel.getValueB("dragging")) {
				Point2f& cur = proj.imagePoints[choice];
				float rate = ofGetMousePressed(0) ? panel.getValueF("slowLerpRate") : panel.getValueF("fastLerpRate");
				cur = Point2f(ofLerp(cur.x, vpX, rate), ofLerp(cur.y, vpY, rate));
				drawLabeledPoint(choice, toOf(cur), pointSize, yellowPrint, ofColor::white, ofColor::black);
				ofSetColor(ofColor::black);
				ofRect(toOf(cur), 1, 1);
			} else if(panel.getValueB("arrowing")) {
				Point2f& cur = proj.imagePoints[choice];
				drawLabeledPoint(choice, toOf(cur), pointSize, yellowPrint, ofColor::white, ofColor::black);
				ofSetColor(ofColor::black);
				ofRect(toOf(cur), 1, 1);
			} else {
				// check to see if anything is selected
				// draw hover magenta
				float distance;



				//ofVec2f selected = toOf(getClosestPoint(proj.imagePoints, mouseX, mouseY, &choice, &distance));
				ofVec2f selected = toOf(getClosestPoint(proj.imagePoints, vpX, vpY, &choice, &distance));
				if(!ofGetMousePressed() && proj.referencePoints[choice] && distance < panel.getValueF("selectionRadius")) {
					panel.setValueI("hoverChoice", choice);
					panel.setValueB("hoverSelected", true);
					drawLabeledPoint(choice, selected, pointSize, magentaPrint);
				} else {
					panel.setValueB("hoverSelected", false);
				}
			} // not dragging or arrowing
		} // if inside viewport
	}

}