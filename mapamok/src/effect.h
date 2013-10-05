#pragma once


class effect
{
public:
	effect()
	{
	}
	virtual ~effect()
	{

	}

	virtual void setupControlPanel(ofxAutoControlPanel& panel) { }
	virtual void update(ofxAutoControlPanel& panel) {}
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model) {}



};