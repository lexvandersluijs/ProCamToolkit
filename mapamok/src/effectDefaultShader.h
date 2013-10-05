#pragma once

class effectDefaultShader : public effect
{
public:

	virtual void setupControlPanel(ofxAutoControlPanel& panel);
	virtual void update(ofxAutoControlPanel& panel);
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model);

private:
	void reloadShaderIfNeeded(ofxAutoControlPanel& panel);

	ofLight light;
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
};