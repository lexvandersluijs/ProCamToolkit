#include "testApp.h"

effectMusicShader1::effectMusicShader1()
{
	shaderPartialFilePath = "shaders/musicshader";
	nrOfPulseFactors = 1;

	initialLightPositions[0].x = 2000.0f;
	initialLightPositions[0].y = 1000.0f;
	initialLightPositions[0].z = 0.0f;

	initialLightPositions[1].x = -2000.0f;
	initialLightPositions[1].y = 3000.0f;
	initialLightPositions[1].z = 0.0f;

	initialLightPositions[2].x = 0.0f;
	initialLightPositions[2].y = 3000.0f;
	initialLightPositions[2].z = -1200.0f;
}

void effectMusicShader1::setupControlPanel(ofxAutoControlPanel& panel)
{
	panel.addMultiToggle("lightmode", 0, variadic("stars")("stripes"));
	panel.addSlider("flowerFactor", 1.f, 0.f, 1.f);
}

void effectMusicShader1::setup()
{
	// look up texture (s) in showDefinition (these have already been 
	// loaded because the resources are always loaded before the segments)
	picture1	= showDefinition::getInstance()->findPictureByName("flowerpattern");
	stars		= showDefinition::getInstance()->findPictureByName("stars");


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
	float t = ofGetElapsedTimef();
	ofVec3f rotAxis(0.1f, 1.0f, 0.1f);

	ofPushStyle();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);

	float color[4];
	color[0] = 0.5f * pulseFactors[0];
	color[1] = 1.0f * pulseFactors[0];
	color[2] = 0.3f * pulseFactors[0];
	color[3] = 1.0f * pulseFactors[0];

	ofMatrix4x4 centerTrans;
	centerTrans.makeTranslationMatrix(256.f, 256.f, 0.f);
	ofMatrix4x4 projectorRot;
	projectorRot.makeRotationMatrix(t * 60.0f, 0.0f, 0.0f, 1.0f); // rotate around projection axis
	ofMatrix4x4 projectorTilt;
	projectorTilt.makeRotationMatrix(30.0f, 1.0f, 0.0f, 0.0f); // tilt over x axis
	ofMatrix4x4 projectorTrans;
	projectorTrans.makeTranslationMatrix(0.0f, 1000.0f, 0.0f);

	//ofMatrix4x4 starsProjectorMatrix = projectorTrans * (projectorTilt * (projectorRot * centerTrans));
	ofMatrix4x4 starsProjectorMatrix = centerTrans * projectorRot; // projectorRot * projectorTilt * projectorTrans;

	centerTrans.makeTranslationMatrix(128.f, 128.f, 0.f);
	projectorRot.makeRotationMatrix(t * 30.0f, 0.0f, 0.0f, 1.0f); // rotate around projection axis
	projectorTrans.makeTranslationMatrix(0.0f, 1000.0f, 0.0f);
	//ofMatrix4x4 flowerProjectorMatrix = projectorTrans * (projectorRot * centerTrans);
	ofMatrix4x4 flowerProjectorMatrix = centerTrans * projectorRot; // * projectorTrans;

	// ------- rotate lights ------
	ofMatrix4x4 rotMat;
	rotMat.makeRotationMatrix(fmod(t*60.0f, 360.0f), rotAxis);
	
	float lightPos[3][3];
	for(int i=0; i<3; i++)
	{
		ofVec3f newPos = initialLightPositions[i] * rotMat;
		lightPos[i][0] = newPos.x;
		lightPos[i][1] = newPos.y;
		lightPos[i][2] = newPos.z;
	}

	float lightCol[3][3];
	lightCol[0][0] = 1.0f;
	lightCol[0][1] = 0.0f;
	lightCol[0][1] = 0.0f;

	lightCol[1][0] = 0.0f;
	lightCol[1][1] = 1.0f;
	lightCol[1][1] = 0.0f;

	lightCol[2][0] = 0.0f;
	lightCol[2][1] = 0.0f;
	lightCol[2][1] = 1.0f;
	// ----------------------------------

	shader.begin();
	shader.setUniform1f("elapsedTime", t);
	shader.setUniform4fv("color", color);
	shader.setUniform3fv("lightPos", (float*)lightPos, 3);
	shader.setUniform3fv("lightCol", (float*)lightCol, 3);
	shader.setUniformTexture("pattern1", *(picture1->getImagePtr()), 1);
	shader.setUniformTexture("stars", *(stars->getImagePtr()), 2);
	shader.setUniform1f("starsFactor", pulseFactors[0]);
	shader.setUniform1f("flowerFactor", panel.getValueF("flowerFactor"));
	shader.setUniform1i("lightMode", panel.getValueI("lightmode"));
	shader.setUniformMatrix4f("starsProjectorMatrix", starsProjectorMatrix.getInverse());
	ofMatrix4x4 flowerInv = flowerProjectorMatrix.getInverse();
	shader.setUniformMatrix4f("flowerProjectorMatrix", flowerInv);
	shader.end();

	shader.begin();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	mesh->drawFaces();

	shader.end();

	glPopAttrib();
	ofPopStyle();
}

