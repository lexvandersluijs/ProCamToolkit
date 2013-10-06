#include "testApp.h"

effectMusicShader1::effectMusicShader1()
{
	shaderPartialFilePath = "shaders/musicshader";
	nrOfPulseFactors = 1;
}

void effectMusicShader1::setupControlPanel(ofxAutoControlPanel& panel)
{
}

void effectMusicShader1::update(ofxAutoControlPanel& panel)
{
	reloadShaderIfNeeded(panel);

	for(int i=0; i<nrOfPulseFactors; i++)
	{
		pulseFactors[i] *= 0.99; // exponential decay.. depends on framerate.. need to adress this
	}
}

void effectMusicShader1::pulse(int index)
{
	pulseFactors[index] = 1.0f;
}

void effectMusicShader1::render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh)
{
	ofPushStyle();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);

	float color[4];
	color[0] = 0.5f * pulseFactors[0];
	color[1] = 1.0f * pulseFactors[0];
	color[2] = 0.3f * pulseFactors[0];
	color[3] = 1.0f * pulseFactors[0];

	shader.begin();
	float t = ofGetElapsedTimef();
	shader.setUniform1f("elapsedTime", t);
	shader.setUniform4fv("color", color);
	shader.end();

	shader.begin();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	mesh->drawFaces();

	shader.end();

	glPopAttrib();
	ofPopStyle();
}

