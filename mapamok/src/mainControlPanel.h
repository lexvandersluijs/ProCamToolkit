#pragma once

class projectorConfiguration;
class showDefinition;

class mainControlPanel : public ofxAutoControlPanel 
{
public:
	void initialize(projectorConfiguration& config, int nrOfPoints, showDefinition& show);
};
