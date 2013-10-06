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
	virtual void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh) {}
	virtual void stop() { }

protected:
	void reloadShaderIfNeeded(ofxAutoControlPanel& panel);

	string shaderPartialFilePath; // without .frag or .vert at the end
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
};