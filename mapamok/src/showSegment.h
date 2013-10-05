#pragma once

class showSegment
{
public:
	// the name of this segment, for clarity in the UI, and for persistence in XML
	string name;

	// a showsegment orchestrates various inputs (e.g. Kinect, audio, computer vision)
	// and translates this into a visualization. A visualization is performed
	// by rendering the model with one or more effects, with different blend states
	// used in the sequence
	std::vector<effect*> effects;

	showSegment()
	{}
	virtual ~showSegment()
	{
		for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
		{
			delete (*eit);
		}
	}
	void setModelAndMesh(ofxAssimpModelLoader* m, ofMesh* me)
	{
		model = m;
		mesh = me;
	}
	virtual void allEffectsSetupControlPanels()
	{
		for(std::vector<effect*>::iterator eit = effects.begin(); eit != effects.end(); ++eit)
		{
			(*eit)->setupControlPanel(controlPanel);
		}
	}
	virtual void setup();
	virtual void start();
	virtual void update();
	virtual void render();
	virtual void end();

	ofxAutoControlPanel& getControlPanel() { return controlPanel; }

protected:

	ofxAssimpModelLoader* model;
	ofMesh* mesh;
	ofxAutoControlPanel controlPanel;
};
