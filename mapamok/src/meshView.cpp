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

void meshView::drawModel(ofPolyRenderMode renderType, ofTexture* textureOverride)
{
	ofPushStyle();
    

	if(!ofGetGLProgrammableRenderer()){
	#ifndef TARGET_OPENGLES
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPolygonMode(GL_FRONT_AND_BACK, ofGetGLPolyMode(renderType));
	#endif
		glEnable(GL_NORMALIZE);
	}
    
	ofEnableNormalizedTexCoords();

	ofPushMatrix();
	//ofMultMatrix(modelMatrix);
    
	for(unsigned int i=0; i<model->getNumMeshes(); i++) {
		ofxAssimpMeshHelper & mesh = model->getMeshHelper(i);
        
		ofPushMatrix();
		//ofMultMatrix(mesh.matrix);

		// if no texture override is specified then try to use the texture from the mesh definition
		if(textureOverride == NULL)
		{
			//if(model.bUsingTextures){
				if(mesh.hasTexture()) {
					ofTexture * tex = mesh.getTexturePtr();
					if(tex->isAllocated()) {
						tex->bind();
					}
				}
			//}
		}

		//if(bUsingMaterials){
			mesh.material.begin();
		//}
        
		if(mesh.twoSided) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
        
		ofEnableBlendMode(mesh.blendMode);
#ifndef TARGET_OPENGLES
		mesh.vbo.drawElements(GL_TRIANGLES,mesh.indices.size());
#else
		switch(renderType){
			case OF_MESH_FILL:
		    	mesh.vbo.drawElements(GL_TRIANGLES,mesh.indices.size());
		    	break;
			case OF_MESH_WIREFRAME:
		    	mesh.vbo.drawElements(GL_LINES,mesh.indices.size());
		    	break;
			case OF_MESH_POINTS:
		    	mesh.vbo.drawElements(GL_POINTS,mesh.indices.size());
		    	break;
		}
#endif

		if(textureOverride == NULL)
		{
			//if(bUsingTextures){
				if(mesh.hasTexture()) {
					ofTexture * tex = mesh.getTexturePtr();
					if(tex->isAllocated()) {
						tex->unbind();
					}
				}
			//}
		}

		//if(bUsingMaterials){
			mesh.material.end();
		//}
        
		ofPopMatrix();
	}
    
	ofPopMatrix();

	if(!ofGetGLProgrammableRenderer()){
	#ifndef TARGET_OPENGLES
		glPopClientAttrib();
		glPopAttrib();
	#endif
	}
	ofPopStyle();
}

void meshView::render(ofxControlPanel& panel, ofLight& light, ofShader& shader, ofImage& customPicture, ofxThreadedVideo& mappingMovie) 
//void meshView::render(ofxControlPanel& panel, ofLight& light, ofShader& shader, ofImage& customPicture, ofVideoPlayer& mappingMovie) 
{
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
		objectMesh->clearColors();
		int n = objectMesh->getNumVertices();
		float highlightPosition = panel.getValueF("highlightPosition");
		float highlightOffset = panel.getValueF("highlightOffset");
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			objectMesh->addColor(cur);
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
			objectMesh->drawFaces();
			if(useShader) shader.end();
	
// use the default implementation from ofxAssimpModelLoader
//			model.drawFaces();

			break;
			}
		case 1: // fullWireframe
			{
			if(useShader) shader.begin();
			objectMesh->drawWireframe();
			if(useShader) shader.end();
			break;
			}
		case 2: // outlineWireframe
			{
			LineArt::draw(*objectMesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
			}
		case 3: // occludedWireframe
			{
			LineArt::draw(*objectMesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
			}
		case 4:
			{
				ofTexture& pictureTexture = customPicture.getTextureReference();
				pictureTexture.bind();
				drawModel(OF_MESH_FILL, &pictureTexture);
				pictureTexture.unbind();
			}
			break;

		case 5: // video
			{
			// use our custom version that supports video textures and stuff
			ofTexture& videoTexture = mappingMovie.getTextureReference();
			videoTexture.bind();
			drawModel(OF_MESH_FILL, &videoTexture);
			videoTexture.unbind();
			break;
			}
	}
	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();
}
