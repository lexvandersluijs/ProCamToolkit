#include "testApp.h"

void enableFog(float nearFog, float farFog) {
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4]= {0, 0, 0, 1};
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogf(GL_FOG_START, nearFog);
	glFogf(GL_FOG_END, farFog);
}

void disableFog() {
	glDisable(GL_FOG);
}

void effectDefaultShader::setupControlPanel(ofxAutoControlPanel& panel)
{
	panel.addMultiToggle("drawMode", 3, variadic("faces")("fullWireframe")("outlineWireframe")("occludedWireframe")("texture"));
	panel.addMultiToggle("shading", 0, variadic("none")("lights")("shader"));

	panel.addSlider("lineWidth", 2, 1, 8, true);
	panel.addToggle("useSmoothing", false);
	panel.addToggle("useFog", false);
	panel.addSlider("fogNear", 200, 0, 1000);
	panel.addSlider("fogFar", 1850, 0, 2500);

	panel.addSlider("lightX", 200, -1000, 1000);
	panel.addSlider("lightY", 400, -1000, 1000);
	panel.addSlider("lightZ", 800, -1000, 1000);
	panel.addToggle("randomLighting", false);

}

void effectDefaultShader::update(ofxAutoControlPanel& panel)
{
	reloadShaderIfNeeded(panel);

	// -------------------- update lighting ---------------------------
	if(panel.getValueB("randomLighting")) {
		panel.setValueF("lightX", ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000);
		panel.setValueF("lightY", ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000);
		panel.setValueF("lightZ", ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000);
	}
	light.setPosition(panel.getValueF("lightX"), panel.getValueF("lightY"), panel.getValueF("lightZ"));
}

void effectDefaultShader::render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model)
{
	ofVboMesh mesh = model->getMesh(0);

	ofPushStyle();
	ofSetLineWidth(panel.getValueI("lineWidth"));
	if(panel.getValueB("useSmoothing")) {
		ofEnableSmoothing();
	} else {
		ofDisableSmoothing();
	}
	int shading = panel.getValueI("shading");
	bool useLights = shading == 1;
	bool useShader = shading == 2;
	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}
	
	if(panel.getValueB("highlight")) {
		mesh.clearColors();
		int n = mesh.getNumVertices();
		float highlightPosition = panel.getValueF("highlightPosition");
		float highlightOffset = panel.getValueF("highlightOffset");
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			mesh.addColor(cur);
		}
	}
	
	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);
	if(useShader) {
		shader.begin();
		shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
		shader.end();
	}
	int nrOfMeshes = 0;

	ofColor transparentBlack(0, 0, 0, 0);
	switch(panel.getValueI("drawMode")) {
		case 0: // faces
			{
			if(useShader) shader.begin();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			mesh.drawFaces();
			if(useShader) shader.end();
			break;
			}
		case 1: // fullWireframe
			{
			if(useShader) shader.begin();
			mesh.drawWireframe();
			if(useShader) shader.end();
			break;
			}
		case 2: // outlineWireframe
			{
			LineArt::draw(mesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
			}
		case 3: // occludedWireframe
			{
			LineArt::draw(mesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
			}
	}
	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();
}


void effectDefaultShader::reloadShaderIfNeeded(ofxAutoControlPanel& panel)
{
	int shading = panel.getValueB("shading");
	bool useShader = shading == 2;
	
	if(useShader) {
		ofFile fragFile("shaders/shader.frag"), vertFile("shaders/shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
			bool validShader = shader.load("shaders/shader");
			panel.setValueB("validShader", validShader);
		}
		lastFragTimestamp = fragTimestamp;
		lastVertTimestamp = vertTimestamp;		
	}

}