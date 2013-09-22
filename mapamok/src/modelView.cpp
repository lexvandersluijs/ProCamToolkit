#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void modelView::draw(ofxControlPanel& panel, float mouseX, float mouseY, projectorView* projView, ofLight& light, ofShader& shader, ofTexture* texture) //ofImage& customPicture, ofxThreadedVideo& mappingMovie) 
//void modelView::draw(ofxControlPanel& panel, float mouseX, float mouseY, projectorView* projView, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie) 
{
	if(objectMesh == NULL)
		return;

	ofSetColor(255);
	cam.begin(viewport); // pass along viewport to prevent another getViewport call
	float scale = panel.getValueF("scale");
	ofScale(scale, scale, scale);
	if(panel.getValueB("useFog")) {
		enableFog(panel.getValueF("fogNear"), panel.getValueF("fogFar"));
	}
	render(panel, light, shader, texture);
	if(panel.getValueB("useFog")) {
		disableFog();
	}
	if(panel.getValueI("mode") == 0) {
		imageMesh = getProjectedInViewportMesh(*objectMesh);
	}
	cam.end();








	if(panel.getValueI("mode") == 0) {
		// draw all points cyan small
		glPointSize(panel.getValueI("screenPointSize"));
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);

		imageMesh.drawVertices();

		int pointSize = panel.getValueI("selectedPointSize");

		// draw all reference points cyan
		int n = projView->proj.referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(projView->proj.referencePoints[i]) {
				drawLabeledPoint(i, imageMesh.getVertex(i), pointSize, cyanPrint);
			}
		}
		
		// check to see if anything is selected
		// draw hover point magenta
		int choice;
		float distance;

		float vpX;
		float vpY;

		if(screenToViewport(mouseX, mouseY, vpX, vpY))
		{
			//ofLogError("mapamok") << "drawSelectionMode() inside viewport " << projView->index;

			ofVec3f selected = getClosestPointOnMesh(imageMesh, vpX, vpY, &choice, &distance);
			//ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
			if(!ofGetMousePressed() && distance < panel.getValueF("selectionRadius")) {
				panel.setValueI("hoverChoice", choice);
				panel.setValueB("hoverSelected", true);
				drawLabeledPoint(choice, selected, pointSize, magentaPrint);
			} else {
				panel.setValueB("hoverSelected", false);
			}


			// draw selected point yellow
			if(panel.getValueB("selected")) {
				int choice = panel.getValueI("selectionChoice");
				ofVec2f selected = imageMesh.getVertex(choice);
				drawLabeledPoint(choice, selected, pointSize, yellowPrint, ofColor::white, ofColor::black);
			}
		}
	}


}