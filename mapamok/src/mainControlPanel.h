#pragma once


class mainControlPanel : public ofxAutoControlPanel 
{
public:
	void initialize(projector& proj)
	{
		setup();
		msg = "tab hides the panel, space toggles render/selection mode, 'f' toggles fullscreen.";
	
		addPanel("Interaction");
		addToggle("setupMode", true);
		addSlider("scale", 1, .1, 25);
		addSlider("backgroundColor", 0, 0, 255, true);
		addMultiToggle("drawMode", 3, variadic("faces")("fullWireframe")("outlineWireframe")("occludedWireframe")("picture"));
		addMultiToggle("shading", 0, variadic("none")("lights")("shader"));
		addToggle("loadCalibration", false);
		addToggle("saveCalibration", false);
	
		addPanel("Show");
		addToggle("playVideo", false);

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
		addSlider("hoverChoice", 0, 0, proj.objectPoints.size(), true);
		addToggle("selected", false);
		addToggle("dragging", false);
		addToggle("arrowing", false);
		addSlider("selectionChoice", 0, 0, proj.objectPoints.size(), true);
		addSlider("slowLerpRate", .001, 0, .01);
		addSlider("fastLerpRate", 1, 0, 1);

	}
};
