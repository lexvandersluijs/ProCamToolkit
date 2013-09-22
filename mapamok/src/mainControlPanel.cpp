#include "testApp.h"

void mainControlPanel::initialize(projectorConfiguration& config, int nrOfPoints, showDefinition& show)
{
	setup();
	msg = "tab hides the panel, space toggles render/selection mode, 'f' toggles fullscreen.";
	
	addPanel("Interaction");

	addMultiToggle("mode", 0, variadic("setup")("show"));

	// dynamic radio button with the list of projectors
	std::vector<std::string, std::allocator<std::string>>* projectors = new std::vector<std::string, std::allocator<std::string>>();
	for(int i=0; i<config.numProjectorViews(); i++)
		projectors->push_back(config.getProjViewPtr(i)->proj.name);
	addMultiToggle("calibrate", 0, *projectors);
	// delete projectors?

//		addToggle("setupMode", true);
	addSlider("scale", 1, .1, 25);
	addSlider("backgroundColor", 0, 0, 255, true);
	addMultiToggle("drawMode", 3, variadic("faces")("fullWireframe")("outlineWireframe")("occludedWireframe")("texture"));
	addMultiToggle("shading", 0, variadic("none")("lights")("shader"));

	addToggle("loadCalibration", false);
	addToggle("saveCalibration", false);
	
	addPanel("Show");
	for(int i=0; i<show.movies.size(); i++)
		addToggle(show.movies[i]->makeGuiName(), false);
	for(int i=0; i<show.pictures.size(); i++)
		addToggle(show.pictures[i]->makeGuiName(), false);

	addPanel("Highlight");
	addToggle("highlight", false);
	addSlider("highlightPosition", 0, 0, 1);
	addSlider("highlightOffset", .1, 0, 1);
	
	addPanel("Calibration");
	addSlider("aov", 80, 50, 100);
	addToggle("CV_CALIB_FIX_ASPECT_RATIO", true);
	addToggle("CV_CALIB_FIX_K1", true);
	addToggle("CV_CALIB_FIX_K2", true);
	addToggle("CV_CALIB_FIX_K3", true);
	addToggle("CV_CALIB_ZERO_TANGENT_DIST", true);
	addToggle("CV_CALIB_FIX_PRINCIPAL_POINT", false);
	
	addPanel("Rendering");
	addSlider("lineWidth", 2, 1, 8, true);
	addToggle("useSmoothing", false);
	addToggle("useFog", false);
	addSlider("fogNear", 200, 0, 1000);
	addSlider("fogFar", 1850, 0, 2500);
	addSlider("screenPointSize", 2, 1, 16, true);
	addSlider("selectedPointSize", 8, 1, 16, true);
	addSlider("selectionRadius", 12, 1, 32);
	addSlider("lightX", 200, -1000, 1000);
	addSlider("lightY", 400, -1000, 1000);
	addSlider("lightZ", 800, -1000, 1000);
	addToggle("randomLighting", false);
	
	addPanel("Internal");
	addToggle("validShader", true);
	addToggle("selectionMode", true);
	addToggle("hoverSelected", false);
	addSlider("hoverChoice", 0, 0, nrOfPoints, true);
	addToggle("selected", false);
	addToggle("dragging", false);
	addToggle("arrowing", false);
	addSlider("selectionChoice", 0, 0, nrOfPoints, true);
	addSlider("slowLerpRate", .05, 0, 0.5);
	addSlider("fastLerpRate", 1, 0, 1);

}