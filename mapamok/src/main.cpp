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

	int primaryScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int primaryScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int virtualScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int virtualScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);


	// get parameter: which configuration to load -> = name of folder
	// if no parameter specified, then create a default configuration
	if(argc == 1)
	{
		projConfig.setName("Default");
		projConfig.setWindowWidth(primaryScreenWidth);
		projConfig.setWindowHeight(primaryScreenHeight);
		projConfig.initialize(primaryScreenWidth, primaryScreenHeight, virtualScreenWidth, virtualScreenHeight);
		projConfig.allocateViewports(1);
		projConfig.getProjView(0).proj.name = "Single";
		projConfig.getProjView(0).index = 0;
		projConfig.getProjView(0).setViewport(0, 0, primaryScreenWidth, primaryScreenHeight);

		show.setModelLocation("models/oefentrap.dae");
		show.addMovie("movies/oefentrap-uvtemplate_2.mov", "oefentrap");
		show.addPicture("pictures/oefentrap-stonesandgrass.png", "stone and grass");

		showSegment* segment = new showSegment();
		segment->name = "Calibration";
		effectSingleTextureMap* calibmap = new effectSingleTextureMap();
		calibmap->addSelectedResourceName("stone and grass");
		segment->effects.push_back(calibmap);
		show.showSegments.push_back(segment);
	}
	else
	{
		string configName(argv[1]);

		if(configName == "FourViewTest")
		{
			float vpWidth = (float)primaryScreenWidth/2; 
			float vpHeight = (float)primaryScreenHeight/2; 

			projConfig.setName("FourViewportsSingleScreen");
			projConfig.setWindowWidth(primaryScreenWidth);
			projConfig.setWindowHeight(primaryScreenHeight);
			projConfig.initialize(primaryScreenWidth, primaryScreenHeight, virtualScreenWidth, virtualScreenHeight);
			projConfig.allocateViewports(4);
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
		else if (configName == "CLCGirls")
		{
			projConfig.initialize(primaryScreenWidth, primaryScreenHeight, virtualScreenWidth, virtualScreenHeight);
			projConfig.loadConfiguration(configName);
			show.load(configName);

			showSegment* segment = new showSegment();
			segment->name = "Calibration";
			effectSingleTextureMap* calibmap = new effectSingleTextureMap();
			calibmap->addSelectedResourceName("calibration");
			segment->effects.push_back(calibmap);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Default";
			effect* ef = new effectDefaultShader();
			segment->effects.push_back(ef);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Welkom";
			effectMultiTextureShader* emts = new effectMultiTextureShader();
			// todo: allow specification of background image.. right now hard coded in effect
			emts->addSelectedResourceName("geenpunt");
			emts->addSelectedResourceName("welkomspunt");
			segment->effects.push_back(emts);
			show.showSegments.push_back(segment);


			segment = new showSegment();
			segment->name = "Waterval";
			effectSingleTextureMap* tmef = new effectSingleTextureMap();
			tmef->addSelectedResourceName("waterval");
			segment->effects.push_back(tmef);
			show.showSegments.push_back(segment);
			
			segment = new showSegmentClcGirls1();
			segment->name = "Meidendans 1";
			effectMusicShader1* musicShader = new effectMusicShader1();
			segment->effects.push_back(musicShader);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Woord";
			tmef = new effectSingleTextureMap();
			tmef->addSelectedResourceName("preekpunt0");
			tmef->addSelectedResourceName("preekpunt1");
			tmef->addSelectedResourceName("preekpunt2");
			tmef->addSelectedResourceName("preekpunt3");
			tmef->addSelectedResourceName("preekpunt4");
			tmef->addSelectedResourceName("preekpunt5");
			tmef->addSelectedResourceName("preekpunt6");
			tmef->addSelectedResourceName("preekpunt7");
			tmef->addSelectedResourceName("preekpunt8");
			tmef->addSelectedResourceName("preekpunt9");
			tmef->addSelectedResourceName("preekpunt10");
			segment->effects.push_back(tmef);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Meidendans 2 (eind)";
			tmef = new effectSingleTextureMap();
			tmef->addSelectedResourceName("einddans");
			segment->effects.push_back(tmef);

			show.showSegments.push_back(segment);

		}
		else if (configName == "Easter2014")
		{
			projConfig.initialize(primaryScreenWidth, primaryScreenHeight, virtualScreenWidth, virtualScreenHeight);
			projConfig.loadConfiguration(configName);
			show.load(configName);

			showSegment* segment = new showSegment();
			segment->name = "Calibration";
			effectSingleTextureMap* calibmap = new effectSingleTextureMap();
			calibmap->addSelectedResourceName("calibration");
			segment->effects.push_back(calibmap);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Default";
			effect* ef = new effectDefaultShader();
			segment->effects.push_back(ef);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Mapping videos";
			effectSingleTextureMap* tmef = new effectSingleTextureMap();
			tmef = new effectSingleTextureMap();
			tmef->addSelectedResourceName("Intro");
			tmef->addSelectedResourceName("Dans");
			tmef->addSelectedResourceName("Girliezz");
			segment->effects.push_back(tmef);
			show.showSegments.push_back(segment);
		}
		else if(configName == "Christmas2016")
		{
			projConfig.initialize(primaryScreenWidth, primaryScreenHeight, virtualScreenWidth, virtualScreenHeight);
			projConfig.loadConfiguration(configName);
			show.load(configName);

			showSegment* segment = new showSegment();
			segment->name = "Calibration";
			effectSingleTextureMap* calibmap = new effectSingleTextureMap();
			calibmap->addSelectedResourceName("calibration");
			segment->effects.push_back(calibmap);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Default";
			effect* ef = new effectDefaultShader();
			segment->effects.push_back(ef);
			show.showSegments.push_back(segment);

			segment = new showSegment();
			segment->name = "Mapping videos";
			effectSingleTextureMap* tmef = new effectSingleTextureMap();
			tmef->addSelectedResourceName("deur_met_begroeing");
			tmef->addSelectedResourceName("Deur_basis_blauw");
			tmef->addSelectedResourceName("Deur_basis_geel");
			tmef->addSelectedResourceName("Deur_basis_groen");
			tmef->addSelectedResourceName("Deur_basis_paars");
			tmef->addSelectedResourceName("Deur_basis_rood");
			tmef->addSelectedResourceName("Scene_2_Heartbeat_loop");
			tmef->addSelectedResourceName("Scene_3_Open");
			tmef->addSelectedResourceName("Scene_4_Open2");
			tmef->addSelectedResourceName("Scene_5_Krimpen");
			tmef->addSelectedResourceName("Scene_6_Groeien");
			segment->effects.push_back(tmef);
			show.showSegments.push_back(segment);
		}
	}


	//ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
	ofSetupOpenGL(&window, projConfig.getWindowWidth(), projConfig.getWindowHeight(), OF_WINDOW);



	ofRunApp(new testApp(projConfig, show));
}
