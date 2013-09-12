#include "testApp.h"

using namespace ofxCv;
using namespace cv;

class ProjectorView
{
public:
	projector* projector;
	ofRectangle viewport;
};

void testApp::setb(string name, bool value) {
	panel.setValueB(name, value);
}
void testApp::seti(string name, int value) {
	panel.setValueI(name, value);
}
void testApp::setf(string name, float value) {
	panel.setValueF(name, value);
}
bool testApp::getb(string name) {
	return panel.getValueB(name);
}
int testApp::geti(string name) {
	return panel.getValueI(name);
}
float testApp::getf(string name) {
	return panel.getValueF(name);
}

void testApp::setup() {

	// ---------------- init viewport for testing -------------------
	projectorViewport.x = 400.0f;
	projectorViewport.y = 100.0f;
	projectorViewport.width = (float)ofGetWidth() - 500.0f;
	projectorViewport.height = (float)ofGetHeight() - 200.0f;
	// ---------------------------------------------------------------

	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
	
	proj.calibrationReady = false;

	// load the 3D model
	setupMesh();

	// let the projecter initialize arrays for calibration from this model
	proj.initializeFromMesh(objectMesh);

	// then initialize the panel, which also needs to know the number of vertices in the mesh..
	panel.initialize(proj);

	// does this fix the TC's?
	// ofEnableNormalizedTexCoords();
	// ofDisableNormalizedTexCoords();


	fingerMovie.loadMovie("movies/fingers.mov");
	fingerMovie.update(); // init the first frame to prevent blueness?

	customPicture0.loadImage("pictures/oefentrap-stonesandgrass.png");

	ofSetWindowTitle("mapamok");
}

void testApp::update() {

	if(getb("playVideo") && fingerMovie.isPlaying() == false)
	{
		fingerMovie.play();
	}
	else
	{
		if(!getb("playVideo") && fingerMovie.isPlaying() == true)
		{
			fingerMovie.stop();
		}
	}
	fingerMovie.update();

	if(getb("randomLighting")) {
		setf("lightX", ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000);
		setf("lightY", ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000);
		setf("lightZ", ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000);
	}
	light.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));
		
	if(getb("selectionMode")) {
		cam.enableMouseInput();
	} else {
		updateRenderMode();
		cam.disableMouseInput();
	}
}

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

// copied from viewportsExample
void testApp::drawViewportOutline(const ofRectangle & viewport){
	ofPushStyle();
	ofFill();
	ofSetColor(50);
	ofSetLineWidth(0);
	ofRect(viewport);
	ofNoFill();
	ofSetColor(25);
	ofSetLineWidth(1.0f);
	ofRect(viewport);
	ofPopStyle();
}

void testApp::draw() {
	ofBackground(geti("backgroundColor"));
    if(getb("loadCalibration")) {
		proj.loadCalibration();
		setb("loadCalibration", false);
	}
	if(getb("saveCalibration")) {
		proj.saveCalibration();
		setb("saveCalibration", false);
	}

	// ----------------- test the viewport thing -------------------
	drawViewportOutline(projectorViewport);

	// keep a copy of your viewport and transform matrices for later
	ofPushView();

	// tell OpenGL to change your viewport. note that your transform matrices will now need setting up
	ofViewport(projectorViewport);

	// setup transform matrices for normal oF-style usage, i.e.
	//  0,0=left,top
	//  ofGetViewportWidth(),ofGetViewportHeight()=right,bottom
	ofSetupScreen();
	// --------------------------------------------------------------



	if(getb("selectionMode")) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}


	// ------------------- unwind the viewport thing ---------------
	// restore the old viewport (now full view and oF coords)
	ofPopView();


	// LS: TODO: this message should be on the main screen, not
	// stretched all over all viewports
	if(!getb("validShader")) {
		ofPushStyle();
		ofSetColor(magentaPrint);
		ofSetLineWidth(8);
		ofLine(0, 0, ofGetWidth(), ofGetHeight());
		ofLine(ofGetWidth(), 0, 0, ofGetHeight());
		string message = "Shader failed to compile.";
		ofVec2f center(ofGetWidth(), ofGetHeight());
		center /= 2;
		center.x -= message.size() * 8 / 2;
		center.y -= 8;
		drawHighlightString(message, center);
		ofPopStyle();
	}

	//fingerMovie.draw(600,20);

}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_LEFT || key == OF_KEY_UP || key == OF_KEY_RIGHT|| key == OF_KEY_DOWN){
		int choice = geti("selectionChoice");
		setb("arrowing", true);
		if(choice > 0){
			Point2f& cur = proj.imagePoints[choice];
			switch(key) {
				case OF_KEY_LEFT: cur.x -= 1; break;
				case OF_KEY_RIGHT: cur.x += 1; break;
				case OF_KEY_UP: cur.y -= 1; break;
				case OF_KEY_DOWN: cur.y += 1; break;
			}
		}
	} else {
		setb("arrowing",false);
	}
	if(key == OF_KEY_BACKSPACE) { // delete selected
		if(getb("selected")) {
			setb("selected", false);
			int choice = geti("selectionChoice");
			proj.referencePoints[choice] = false;
			proj.imagePoints[choice] = Point2f();
		}
	}
	if(key == '\n') { // deselect
		setb("selected", false);
	}
	if(key == ' ') { // toggle render/select mode
		setb("selectionMode", !getb("selectionMode"));
	}
}

void testApp::mousePressed(int x, int y, int button) {
	setb("selected", getb("hoverSelected"));
	seti("selectionChoice", geti("hoverChoice"));
	if(getb("selected")) {
		setb("dragging", true);
	}
}

void testApp::mouseReleased(int x, int y, int button) {
	setb("dragging", false);
}

void testApp::setupMesh() {
	//model.loadModel("model.dae");
	//model.loadModel("movicolon-box.dae");
	//model.loadModel("oefentrap.obj");
	model.loadModel("oefentrap.dae");
	
	// LS: note: stuff breaks down when there is more than one submesh in the model
	objectMesh = model.getMesh(0);
}

// LvdS: adapted from ofxAssimpModelLoader
void testApp::drawModel(ofPolyRenderMode renderType)
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
    
	for(unsigned int i=0; i<model.getNumMeshes(); i++) {
		ofxAssimpMeshHelper & mesh = model.getMeshHelper(i);
        
        ofPushMatrix();
        //ofMultMatrix(mesh.matrix);

		ofTexture& videoTexture = fingerMovie.getTextureReference();
		ofTexture& pictureTexture = customPicture0.getTextureReference();
		int textureMode = 1;
		// 0 = texture referenced by mesh
		// 1 = override picture
		// 2 = override video
		switch(textureMode)
		{
		case 0:
			//if(model.bUsingTextures){
				if(mesh.hasTexture()) {
					ofTexture * tex = mesh.getTexturePtr();
					if(tex->isAllocated()) {
						tex->bind();
					}
				}
			//}

			break;
		case 1:
			pictureTexture.bind();
			break;
		case 2:
			videoTexture.bind();
			break;
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

		switch(textureMode)
		{
		case 0:
        //if(bUsingTextures){
            if(mesh.hasTexture()) {
                ofTexture * tex = mesh.getTexturePtr();
                if(tex->isAllocated()) {
                    tex->unbind();
                }
            }
        //}
		case 1:
			pictureTexture.unbind();
			break;	
		case 2:
			videoTexture.unbind();
			break;
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

void testApp::render() {
	ofPushStyle();
	ofSetLineWidth(geti("lineWidth"));
	if(getb("useSmoothing")) {
		ofEnableSmoothing();
	} else {
		ofDisableSmoothing();
	}
	int shading = geti("shading");
	bool useLights = shading == 1;
	bool useShader = shading == 2;
	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}
	
	if(getb("highlight")) {
		objectMesh.clearColors();
		int n = objectMesh.getNumVertices();
		float highlightPosition = getf("highlightPosition");
		float highlightOffset = getf("highlightOffset");
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			objectMesh.addColor(cur);
		}
	}
	
	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);
	if(useShader) {
		ofFile fragFile("shader.frag"), vertFile("shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
			bool validShader = shader.load("shader");
			setb("validShader", validShader);
		}
		lastFragTimestamp = fragTimestamp;
		lastVertTimestamp = vertTimestamp;
		
		shader.begin();
		shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
		shader.end();
	}
	int nrOfMeshes = 0;

	ofColor transparentBlack(0, 0, 0, 0);
	switch(geti("drawMode")) {
		case 0: // faces
			if(useShader) shader.begin();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			objectMesh.drawFaces();
			if(useShader) shader.end();
	
// use the default implementation from ofxAssimpModelLoader
//			model.drawFaces();

// use our custom version that supports video textures and stuff
//			drawModel(OF_MESH_FILL);
			break;
		case 1: // fullWireframe
			if(useShader) shader.begin();
			objectMesh.drawWireframe();
			if(useShader) shader.end();
			break;
		case 2: // outlineWireframe
			LineArt::draw(objectMesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
		case 3: // occludedWireframe
			LineArt::draw(objectMesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
		case 4: // picture
			drawModel(OF_MESH_FILL);
			break;
	}
	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();
}

void testApp::updateRenderMode() {
	float aov = getf("aov");

	// generate flags
	#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
	int flags =
		CV_CALIB_USE_INTRINSIC_GUESS |
		getFlag(CV_CALIB_FIX_PRINCIPAL_POINT) |
		getFlag(CV_CALIB_FIX_ASPECT_RATIO) |
		getFlag(CV_CALIB_FIX_K1) |
		getFlag(CV_CALIB_FIX_K2) |
		getFlag(CV_CALIB_FIX_K3) |
		getFlag(CV_CALIB_ZERO_TANGENT_DIST);

	proj.updateCalibration(aov, flags);

}

void testApp::drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);

	//float w = ofGetWidth();
	//float h = ofGetHeight();

	// LS: make compatible with multi-viewport operation
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	float w = (float)viewport[2];
	float h = (float)viewport[3];

	ofVec2f newPos = position;
	//newPos.x -= (float)viewport[0];
	//newPos.y -= (float)viewport[1];

	ofSetLineWidth(1.5);
	ofLine(newPos - ofVec2f(w,0), newPos + ofVec2f(w,0));
	ofLine(newPos - ofVec2f(0,h), newPos + ofVec2f(0,h));
	ofCircle(newPos, geti("selectedPointSize"));
	drawHighlightString(ofToString(label), newPos + tooltipOffset, bg, fg);
	glPopAttrib();
}

// function that maps a screen coordinate to the current viewport. Returns false
// if the coordinate is outside the viewport, true if inside
bool screenToViewport(float mouseX, float mouseY, float& vpX, float& vpY)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	if((mouseX > viewport[0]) && (mouseX < (viewport[0] + viewport[2])) &&
		(mouseY > viewport[1]) && (mouseY < (viewport[1] + viewport[3])))
	{
		vpX = mouseX - viewport[0];
		vpY = mouseY - viewport[1];
		return true;
	}
	return false;
}

void testApp::drawSelectionMode() {
	ofSetColor(255);
	//cam.begin();
	cam.begin(projectorViewport); // LS
	float scale = getf("scale");
	ofScale(scale, scale, scale);
	if(getb("useFog")) {
		enableFog(getf("fogNear"), getf("fogFar"));
	}
	render();
	if(getb("useFog")) {
		disableFog();
	}
	if(getb("setupMode")) {
		//imageMesh = getProjectedMesh(objectMesh);
		imageMesh = getProjectedInViewportMesh(objectMesh);
	}
	cam.end();








	if(getb("setupMode")) {
		// draw all points cyan small
		glPointSize(geti("screenPointSize"));
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);

		// why vertices not in correct spot?
		//ofRectangle tempViewport;
		//tempViewport.x = 0;
		//tempViewport.y = 0;
		//tempViewport.width = ofGetWidth();
		//tempViewport.height = ofGetHeight();

		//ofViewport(tempViewport);
		//ofSetupScreen();

		imageMesh.drawVertices();

		// switch back..
		//ofViewport(projectorViewport);
		//ofSetupScreen();

		// draw all reference points cyan
		int n = proj.referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(proj.referencePoints[i]) {
				drawLabeledPoint(i, imageMesh.getVertex(i), cyanPrint);
			}
		}
		
		// check to see if anything is selected
		// draw hover point magenta
		int choice;
		float distance;

		float vpX;
		float vpY;
		screenToViewport(mouseX, mouseY, vpX, vpY);
		ofVec3f selected = getClosestPointOnMesh(imageMesh, vpX, vpY, &choice, &distance);
		//ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
		if(!ofGetMousePressed() && distance < getf("selectionRadius")) {
			seti("hoverChoice", choice);
			setb("hoverSelected", true);
			drawLabeledPoint(choice, selected, magentaPrint);
		} else {
			setb("hoverSelected", false);
		}
		
		// draw selected point yellow
		if(getb("selected")) {
			int choice = geti("selectionChoice");
			ofVec2f selected = imageMesh.getVertex(choice);
			drawLabeledPoint(choice, selected, yellowPrint, ofColor::white, ofColor::black);
		}
	}


}

void testApp::drawRenderMode() {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(proj.calibrationReady) {
		proj.intrinsics.loadProjectionMatrix(10, 2000);
		applyMatrix(proj.modelMatrix);
		render();
		if(getb("setupMode")) {
//			imageMesh = getProjectedMesh(objectMesh);	
			imageMesh = getProjectedInViewportMesh(objectMesh);	
		}
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(getb("setupMode")) {

		float vpX;
		float vpY;
		screenToViewport(mouseX, mouseY, vpX, vpY);

		// draw all reference points cyan
		int n = proj.referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(proj.referencePoints[i]) {
				drawLabeledPoint(i, toOf(proj.imagePoints[i]), cyanPrint);
			}
		}
		
		// move points that need to be dragged
		// draw selected yellow
		int choice = geti("selectionChoice");
		if(getb("selected")) {
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
		if(getb("dragging")) {
			Point2f& cur = proj.imagePoints[choice];
			float rate = ofGetMousePressed(0) ? getf("slowLerpRate") : getf("fastLerpRate");
			cur = Point2f(ofLerp(cur.x, vpX, rate), ofLerp(cur.y, vpY, rate));
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
		} else if(getb("arrowing")) {
			Point2f& cur = proj.imagePoints[choice];
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
        } else {
			// check to see if anything is selected
			// draw hover magenta
			float distance;



			//ofVec2f selected = toOf(getClosestPoint(proj.imagePoints, mouseX, mouseY, &choice, &distance));
			ofVec2f selected = toOf(getClosestPoint(proj.imagePoints, vpX, vpY, &choice, &distance));
			if(!ofGetMousePressed() && proj.referencePoints[choice] && distance < getf("selectionRadius")) {
				seti("hoverChoice", choice);
				setb("hoverSelected", true);
				drawLabeledPoint(choice, selected, magentaPrint);
			} else {
				setb("hoverSelected", false);
			}
		}
	}
}
