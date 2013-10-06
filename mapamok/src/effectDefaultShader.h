#pragma once

class effectDefaultShader : public effect
{
public:
	effectDefaultShader();
	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);

private:


	ofLight light;

};