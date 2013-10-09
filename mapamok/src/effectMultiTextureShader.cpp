#include "testApp.h"

effectMultiTextureShader::effectMultiTextureShader()
{
	shaderPartialFilePath = "shaders/multitexture";	
}

void effectMultiTextureShader::setup()
{
	background	= showDefinition::getInstance()->findPictureByName("edges");
	wavefront	= showDefinition::getInstance()->findPictureByName("wavefront");
}

void effectMultiTextureShader::update(ofxAutoControlPanel& panel)
{
	effectSingleTextureMap::update(panel);

	reloadShaderIfNeeded(panel);
}

void effectMultiTextureShader::render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh)
{
	ofPushStyle();

	
	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);

	pictureResource* overlay = static_cast<pictureResource*>(currentResource);
	if(overlay != NULL)
	{
		float t = ofGetElapsedTimef();

		float backgroundFactor = 1.f;
		float overlayFactor = 1.f;

		shader.begin();
		shader.setUniform1f("elapsedTime", t);
		shader.setUniform1f("backgroundFactor", backgroundFactor);
		shader.setUniform1f("overlayFactor", overlayFactor);
		shader.setUniformTexture("background", *(background->getImagePtr()), 1);
		shader.setUniformTexture("overlay", *(overlay->getImagePtr()), 2);
		shader.setUniformTexture("wavefront", *(wavefront->getImagePtr()), 3);
		shader.end();

		shader.begin();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		mesh->drawFaces();

		shader.end();
	}

	glPopAttrib();

	ofPopStyle();

}