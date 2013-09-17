#pragma once

class projectorConfiguration;

class mainControlPanel : public ofxAutoControlPanel 
{
public:
	void initialize(projectorConfiguration& config, int nrOfPoints);
};
