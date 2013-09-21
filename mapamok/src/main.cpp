#include "testApp.h"
#include "ofAppGlutWindow.h"



// TODO: accept command line parameters .. OR: a configuration file.. and then one parameter: which configuration to load!
// for example: -singleScreen (selection and render view toggle)
// -viewportWidth, -viewportHeight (specify the size of a single projector viewport, it is the user's
//    responsibility to make sure that all screens are running at this resolution)
// -numberOfProjectors. If -singleScreen is not specified, it is implied that projector nr 1 goes to the right of
///   the main control screen, projector nr 2 to the right of _that_, etc
// the startup code will create a window of the correct size to house all these vieports, hide the window's titlebar and
//    move it to 0,0

// -also: specify model to load, movie(s) to load, picture(s) to load
//   possibly also: shaders to load, etc. addon-effects might be in DLL's as 

// nice: if numberOfProjectors > 1 and -singleScreen also specified, tile the viewports for the projectors in a grid, and keep
// the selection viewport in sync for easy testing: if the selected points end up in very different locations in 
// these viewports it is hard to line them up. This combination is only used for testing anyway.
// usually -singleScreen implies -numberOfProjectors=1, then we go back to the original mapamok way of working

// note: current (virtual) display resolution in Windows: http://stackoverflow.com/questions/4631292/how-detect-current-screen-resolution



// ------------------------- start it up ----------------------------
int main( int argc,      // Number of strings in array argv
          char *argv[],   // Array of command-line argument strings
          char *envp[] ) 
{

	ofAppGlutWindow window;
#ifdef TARGET_OSX
	window.setGlutDisplayString("rgba double samples>=8 depth");
#endif

	projectorConfiguration projConfig;
	showDefinition show;

	// TODO: make command line parameter for this
	ofLogLevel(OF_LOG_VERBOSE);


	// get parameter: which configuration to load -> = name of folder
	// if no parameter specified, then create a default configuration
	if(argc == 1)
	{
		float vpWidth = (float)GetSystemMetrics(SM_CXSCREEN); // ofGetScreenWidth()/2; doesn't work, uses window, isn't there yet..
		float vpHeight = (float)GetSystemMetrics(SM_CYSCREEN); //ofGetScreenHeight()/2;
		projConfig.setName("Default");
		projConfig.initialize(1);
		projConfig.getProjView(0).proj.name = "Single";
		projConfig.getProjView(0).index = 0;
		projConfig.getProjView(0).setViewport(0, 0, vpWidth, vpHeight);

		show.addMovie("movies/fingers.mov", "fingers");
		show.addPicture("pictures/oefentrap-stonesandgrass.png", "stone and grass");
	}
	else
	{
		string configName(argv[1]);

		if(configName == "FourViewTest")
		{
			float vpWidth = (float)GetSystemMetrics(SM_CXSCREEN) / 2; // ofGetScreenWidth()/2; doesn't work, uses window, isn't there yet..
			float vpHeight = (float)GetSystemMetrics(SM_CYSCREEN) / 2; //ofGetScreenHeight()/2;
			projConfig.setName("FourViewportsSingleScreen");
			projConfig.initialize(4);
			projConfig.getProjView(0).proj.name = "Top left";
			projConfig.getProjView(0).index = 0;
			projConfig.getProjView(0).setViewport(0, 0, vpWidth, vpHeight);
			projConfig.getProjView(1).index = 1;
			projConfig.getProjView(1).proj.name = "Top right";
			projConfig.getProjView(1).setViewport(vpWidth, 0, vpWidth, vpHeight);
			projConfig.getProjView(2).index = 2;
			projConfig.getProjView(2).proj.name = "Bottom left";
			projConfig.getProjView(2).setViewport(0, vpHeight, vpWidth, vpHeight);
			projConfig.getProjView(3).index = 3;
			projConfig.getProjView(3).proj.name = "Bottom right";
			projConfig.getProjView(3).setViewport(vpWidth, vpHeight, vpWidth, vpHeight);
			projConfig.setViewToCalibrate(0);

			show.addMovie("movies/fingers.mov", "fingers");
			show.addPicture("pictures/oefentrap-stonesandgrass.png", "stone and grass");
		}
		else
		{
			projConfig.loadConfiguration(configName);
			show.load(configName);

			// Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::WhatToShow::SHOW_ELEMENT);
			//Poco::XML::Node* pNode = it.nextNode();
			//while (pNode)
			//{
			// std::cout<<pNode->nodeName()<<":"<< pNode->nodeValue()<<std::endl;
			// pNode = it.nextNode();
			//}

			// default, single monitor configuration: one projector, one screen
			//projConfig.initialize(1);
			//projConfig.getProjView(0).viewport.x = 0;
			//projConfig.getProjView(0).viewport.y = 0;
			//projConfig.getProjView(0).viewport.width = ofGetWidth();
			//projConfig.getProjView(0).viewport.height = ofGetHeight();
			//projConfig.setViewToCalibrate(0);

			// testing: four projector views on a single monitor
		}
	}

	// todo: production:
	// multiple views laid out on large extended desktop


	ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
	//ofSetupOpenGL(&window, 2048, 768, OF_WINDOW);
	ofRunApp(new testApp(projConfig, show));
}
