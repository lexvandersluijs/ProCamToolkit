#include "testApp.h"

void effect::reloadShaderIfNeeded(ofxAutoControlPanel& panel)
{
	ofFile fragFile(shaderPartialFilePath + ".frag"), vertFile(shaderPartialFilePath + ".vert");
	Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
	Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
	if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
		bool validShader = shader.load(shaderPartialFilePath);
		panel.setValueB("validShader", validShader);
	}
	lastFragTimestamp = fragTimestamp;
	lastVertTimestamp = vertTimestamp;		

}