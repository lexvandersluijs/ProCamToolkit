#include "testApp.h"

using namespace ofxCv;
using namespace cv;



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

	bool showWindowBorder = false;
	if (!showWindowBorder) {
	  HWND m_hWnd = WindowFromDC(wglGetCurrentDC());
	  long style = ::GetWindowLong(m_hWnd, GWL_STYLE);
	  style &= ~WS_DLGFRAME;
	  style &= ~WS_CAPTION;
	  style &= ~WS_BORDER;
	  style &= WS_POPUP;
	  _originalWindowStyle = style;
	  ::SetWindowLong(m_hWnd, GWL_STYLE, style);

	  long exstyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	  exstyle &= ~WS_EX_DLGMODALFRAME;
	  _originalWindowStyleEx = exstyle;
	  ::SetWindowLong(m_hWnd, GWL_EXSTYLE, exstyle);

	  //SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
	}

	// TODO: instead of hardcoding, can we get a the main physical screen's width in OF,
	// instead of the window size or virtual desktop size?
	selectionView.setViewport(0, 0, projConfig.getPrimaryScreenWidth(), projConfig.getPrimaryScreenHeight()); 

	// ---------------------------------------------------------------

	//ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
	ofSetVerticalSync(false); //true);
	

	// load the 3D model
	setupMesh();

	// let the projectors initialize arrays for calibration from this model
	// and also let the projector- and selection-views know about it, so that
	// they can render it
	for(int i=0; i<projConfig.numProjectorViews(); i++)
	{
		projConfig.getProjView(i).setModelAndMesh(&model, &objectMesh);
	}
	selectionView.setModelAndMesh(&model, &objectMesh);

	// then initialize the panel, which also needs to know the number of vertices in the mesh..
	panel.initialize(projConfig, objectMesh.getNumVertices(), show);
	
	// load all resources, let all showsegments initialize themselves:
	show.setup();

	ofSetWindowTitle("mapamok");
}

void testApp::update() 
{
	// possibly update the current segment selection
	int selectedSegmentIndex = panel.getValueI("segments");
	if(selectedSegmentIndex != show.currentSegmentIndex)
	{
		if(show.currentSegment != NULL)
			show.currentSegment->end();

		show.setCurrentSegment(selectedSegmentIndex);

		if(show.currentSegment != NULL)
			show.currentSegment->start();
	}
	// and have the current segment update itself. this will call on all its (active) embedded effects to update themselves
	show.update(panel);



	
	// --------------- if we are in selection mode, input should go the main 3D model view ----
	if(getb("selectionMode")) 
		selectionView.cam.enableMouseInput();
	else
	{
		selectionView.cam.disableMouseInput();

		// only update the calibration while we are in Setup mode, not in Show mode..
		if(geti("mode") == 0)
		{
			// if we're not selecting, then we are working in the render view and the calibration can be updated
			// TODO: only for the projector being edited, and only if something has actually changed
			// can save many cycles..
			//for(int i=0; i<projConfig.numProjectorViews(); i++)
			//{
			//	projectorView* projView = projConfig.getProjViewPtr(i);
			//	projView->updateCalibration(panel);
			//}
			if(projConfig.getViewToCalibrate() != NULL)
				projConfig.getViewToCalibrate()->updateCalibration(panel);
		}
	}
	// --------------- check if we should load or save a calibration -----
	if(getb("loadCalibration")) 
	{
		projConfig.loadCalibration();
		setb("loadCalibration", false);
	}
	if(getb("saveCalibration")) 
	{
		projConfig.saveCalibration();
		setb("saveCalibration", false);
	}

	// if 'None' is selected in the list of projectors for calibration, we set NULL
	// as the current pointer
	// otherwise the index we want to set is equal to the number we got from the GUI
	// minus 1, since they were added in the same order
	int projToCalibrateIndex = geti("calibrate");
	projConfig.setViewToCalibrate(projToCalibrateIndex);
}



// copied from viewportsExample
void testApp::drawViewportOutline(const ofRectangle & viewport){
	ofPushStyle();
	ofFill();
	ofSetColor(0);
	ofSetLineWidth(0);
	ofRect(viewport);
	ofNoFill();
	if(geti("mode") == 0) // only draw viewport borders in setup mode
	{
		ofSetColor(128);
		ofSetLineWidth(1.0f);
		ofRect(viewport);
	}
	ofPopStyle();
}


void testApp::draw() {
	//ofBackground(geti("backgroundColor"));

	show.currentSegment->setModelAndMesh(&model, &objectMesh);

	// in selectionmode we render the 3D object (TODO: full-screen) using the easyCam of the 
	// viewport we are calibrating. this is on the main GUI monitor
	if(getb("selectionMode")) 
	{
		projectorView* projViewToCalibrate = projConfig.getViewToCalibrate();
		if(projViewToCalibrate != NULL)
		{
			ofRectangle vp = projViewToCalibrate->getViewport();

			// .. so: if the viewport is on the primary screen, this means we either have a single-viewport (original mapamok) situation
			// or a testing config, where we have laid out multiple viewports on the main screen. in those cases we override the default
			// viewport settings of the selectionView (as being on the main screen) and put this viewport in the same location as the
			// projection view
			if(vp.x < projConfig.getPrimaryScreenWidth())
				selectionView.setViewport(vp.x, vp.y, vp.width, vp.height);

			ofRectangle selectionVp = selectionView.getViewport();
			// ----------------- draw the viewport -------------------
			drawViewportOutline(selectionVp);

			// keep a copy of your viewport and transform matrices for later
			ofPushView();

			// tell OpenGL to change your viewport. note that your transform matrices will now need setting up
			ofViewport(selectionVp);

			// setup transform matrices for normal oF-style usage, i.e.
			//  0,0=left,top
			//  ofGetViewportWidth(),ofGetViewportHeight()=right,bottom
			ofSetupScreen();
			//setupScreen_custom(0, 0, 60, 0, 0); 
			// --------------------------------------------------------------

			selectionView.draw(panel, mouseX, mouseY, projViewToCalibrate, show.currentSegment); 

			// ------------------- unwind the viewport thing ---------------
			// restore the old viewport (now full view and oF coords)
			ofPopView();
		}

		// else: if we are not calibrating, we show nothing?
		// .. no.. then we should probably see the main Show GUI
		// although we could consider showing the 3D model in the background
		// so that the mapping etc can be checked in real-time, from the operator
		// station
		// .. TODO
	}
	else
	{
		// in render-mode we render all the views
		// except in selectionMode, then we only render the selected view

		bool setupMode = (geti("mode") == 0);
		for(int i=0; i<projConfig.numProjectorViews(); i++)
		{
			projectorView* projView = projConfig.getProjViewPtr(i);

			if((setupMode && projView == projConfig.getViewToCalibrate()) || !setupMode)
			{
				// ----------------- draw the viewport -------------------
				drawViewportOutline(projView->getViewport());

				// keep a copy of your viewport and transform matrices for later
				ofPushView();

				// tell OpenGL to change your viewport. note that your transform matrices will now need setting up
				ofViewport(projView->getViewport());

				// setup transform matrices for normal oF-style usage, i.e.
				//  0,0=left,top
				//  ofGetViewportWidth(),ofGetViewportHeight()=right,bottom
				ofSetupScreen();
				// --------------------------------------------------------------

				projView->draw(panel, mouseX, mouseY, show.currentSegment); 

				// ------------------- unwind the viewport thing ---------------
				// restore the old viewport (now full view and oF coords)
				ofPopView();
			}
		}

		// if we do not render the selection view in the same spot as the projection
		// view, then we can render this view side-by-side with the projectionviews
		ofRectangle vp = selectionView.getViewport();
		if(!(vp.x < projConfig.getPrimaryScreenWidth()))
		{
			// ----------------- draw the viewport -------------------
			drawViewportOutline(vp);

			// keep a copy of your viewport and transform matrices for later
			ofPushView();

			// tell OpenGL to change your viewport. note that your transform matrices will now need setting up
			ofViewport(vp);

			// setup transform matrices for normal oF-style usage, i.e.
			//  0,0=left,top
			//  ofGetViewportWidth(),ofGetViewportHeight()=right,bottom
			ofSetupScreen();
			//setupScreen_custom(0, 0, 60, 0, 0); 
			// --------------------------------------------------------------

			selectionView.draw(panel, mouseX, mouseY, NULL, show.currentSegment); 

			// ------------------- unwind the viewport thing ---------------
			// restore the old viewport (now full view and oF coords)
			ofPopView();
		}

	}

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

	//mappingMovie.draw(600,20);

}

// TODO: this direct access to data of the projectorView should be refactored..
// the projectorView itself should probably handle most of these events
void testApp::keyPressed(int key) 
{
	projectorView* viewToCalibrate = projConfig.getViewToCalibrate();
	if(viewToCalibrate != NULL)
	{
		if(key == OF_KEY_LEFT || key == OF_KEY_UP || key == OF_KEY_RIGHT|| key == OF_KEY_DOWN){
			int choice = geti("selectionChoice");
			setb("arrowing", true);
			if(choice > 0){
				Point2f& cur = viewToCalibrate->proj.imagePoints[choice];
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
		// contrary to standard behaviour of ofxAutoControlPanel, we toggle the visibility here
		// instead of letting the control panels toggle themselves (we want to control the show/hide behaviour explicitly)
		if(key == OF_KEY_TAB)
		{
			if(panel.hidden)
			{
				panel.show();
				if(show.currentSegment != NULL)
					show.currentSegment->getControlPanel().show();
			}
			else
			{
				panel.hide();
				if(show.currentSegment != NULL)
					show.currentSegment->getControlPanel().hide();
			}
		}
		if(key == OF_KEY_RETURN)
		{
			ofLogError("mapamok") << "Enter key was pressed";
			int choice = geti("selectionChoice");
			if(choice > 0)
			{
				float vpX, vpY;
				viewToCalibrate->screenToViewport((float)lastMouseX, (float)lastMouseY, vpX, vpY);
				ofLogError("Enter key was pressed") << "Snapping point to " << vpX << ", " << vpY;
				viewToCalibrate->proj.imagePoints[choice].x = vpX;
				viewToCalibrate->proj.imagePoints[choice].y = vpY;
			}
		}
		if(key == OF_KEY_BACKSPACE) { // delete selected
			if(getb("selected")) {
				setb("selected", false);
				int choice = geti("selectionChoice");
				viewToCalibrate->proj.referencePoints[choice] = false;
				viewToCalibrate->proj.imagePoints[choice] = Point2f();
			}
		}
		if(key == OF_KEY_ESC)
		{
			// restore the window state before exiting
			HWND hWnd = WindowFromDC(wglGetCurrentDC());
			::SetWindowLong(hWnd, GWL_STYLE, _originalWindowStyle);
			::SetWindowLong(hWnd, GWL_EXSTYLE, _originalWindowStyleEx);
			ofBaseApp::keyPressed(key);

		}
		if(key == '\n') { // deselect
			setb("selected", false);
		}
		if(key == ' ') { // toggle render/select mode
			setb("selectionMode", !getb("selectionMode"));
		}
	}
}

void testApp::mouseMoved(int x, int y)
{
	lastMouseX = x;
	lastMouseY = y;
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
	//model.loadModel("oefentrap.dae");
	model.loadModel("models/stairs.dae");
	
	// LS: note: stuff breaks down when there is more than one submesh in the model
	objectMesh = model.getMesh(0);
}

