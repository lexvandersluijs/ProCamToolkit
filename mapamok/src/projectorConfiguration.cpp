#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void projectorConfiguration::saveCalibration()
{
	// create a base name for the calibration file(s), based on the name of this configuration
	string dirName = name + "/";
	ofDirectory dir(dirName); // this is relative to the current working folder, apparently set to <programfolder>\data by OF
	dir.create();

	// pass this on to each projector and have it save its own data
	// each projector will check if this file already exists, and overwrite it if so
	for(int i=0; i<nrOfProjViews; i++)
		projViews[i].proj.saveCalibration(dirName);
}

void projectorConfiguration::loadCalibration()
{
	// create a base name for the calibration file(s), based on the name of this configuration
	string dirName = name + "/";

	// pass this on to each projector and have it save its own data
	for(int i=0; i<nrOfProjViews; i++)
		projViews[i].proj.loadCalibration_fromConfigFolder(dirName);

}