#include "testApp.h"

effectMusicShader1::effectMusicShader1()
{
	shaderPartialFilePath = "shaders/musicshader";
	nrOfPulseTimes = 5;
	for(int i=0; i<nrOfPulseTimes; ++i)
	{
		pulseTimes[i] = 0.f;
		pulseFactors[i] = 0.f;
	}

	lightingMode = 0;
	centralPatternIndex = 0;
	centralPatternMotion = 0;

	initialLightPositions[0].x = 200.0f;
	initialLightPositions[0].y = 100.0f;
	initialLightPositions[0].z = 0.0f;

	initialLightPositions[1].x = -150.0f;
	initialLightPositions[1].y = 300.0f;
	initialLightPositions[1].z = 0.0f;

	initialLightPositions[2].x = 0.0f;
	initialLightPositions[2].y = 300.0f;
	initialLightPositions[2].z = -200.0f;

	picture1 = NULL;
	heart = NULL;
	stars = NULL;

	prevLightStepIndex = 0;
	lightBarLowY = 0.f;
	lightBarHighY = 0.f;

	mainLightsLevel = 1.f;
	centralPatternLevel = 1.f;

	controlPanel = NULL; 
}

void effectMusicShader1::setupControlPanel(ofxAutoControlPanel& panel)
{
	controlPanel = &panel;

	panel.addMultiToggle("lightmode", 0, variadic("stars")("stripes")("lightbars"));
	panel.addMultiToggle("pattern", 0, variadic("flower")("heart"));
	panel.addMultiToggle("motion", 0, variadic("none")("thump")("rotate"));
	panel.addSlider("flowerFactor", 1.f, 0.f, 1.f);
	panel.addSlider("avg", 0.f, 0.f, 1.f);
	panel.addSlider("low", 0.f, 0.f, 1.f);
	panel.addSlider("mid", 0.f, 0.f, 1.f);
	panel.addSlider("high", 0.f, 0.f, 1.f);
}

void effectMusicShader1::setup()
{
	// look up texture (s) in showDefinition (these have already been 
	// loaded because the resources are always loaded before the segments)
	picture1	= showDefinition::getInstance()->findPictureByName("flowerpattern");
	heart		= showDefinition::getInstance()->findPictureByName("heart");
	stars		= showDefinition::getInstance()->findPictureByName("stars");


}

void effectMusicShader1::setCentralPatternLevel(float level)
{
	centralPatternLevel = level;
	controlPanel->setValueF("flowerFactor", centralPatternLevel);
}

void effectMusicShader1::setLightingMode(int mode)
{
	lightingMode = mode;
	controlPanel->setValueI("lightmode", lightingMode);
}

void effectMusicShader1::setCentralPattern(int patternIndex)
{
	centralPatternIndex = patternIndex;
	controlPanel->setValueI("pattern", centralPatternIndex);
}

void effectMusicShader1::setCentralPatternMotion(int motionIndex) // 0=none, 1=thumping, 2=rotating
{
	centralPatternMotion = motionIndex;
	controlPanel->setValueI("motion", motionIndex);
}

void effectMusicShader1::update(ofxAutoControlPanel& panel)
{
	reloadShaderIfNeeded(panel);
	
	const float MAX_PULSE_DURATION = 5.f;

	float t = ofGetElapsedTimef();
	for(int i=0; i<nrOfPulseTimes; i++)
	{
		float timeDif = t - pulseTimes[i];
		if(timeDif > 0 && timeDif < MAX_PULSE_DURATION)
			pulseFactors[i] = pow(0.001f, timeDif * 2); // exponential decay.. 
		else
			pulseFactors[i] = 0;
	}

	centralPatternIndex = panel.getValueI("pattern");
	centralPatternMotion = panel.getValueI("motion");
}

void effectMusicShader1::pulse(int index)
{
	pulseTimes[index] = ofGetElapsedTimef();
	pulseFactors[index] = 1.0f;

	// for the lightbars mode
	if(index == 2) // low always triggers
	{
		// select a new random stairstep, and make sure it's different from what we had before
		int lightStepIndex;
		do
		{
			lightStepIndex = (int)ofRandom(10);
		}
		while(lightStepIndex == prevLightStepIndex);
		
		lightBarLowY = (float)lightStepIndex * 19.f + 0.5f;
		lightBarHighY = (float)(lightStepIndex+1) * 19.f + 0.4f;

		prevLightStepIndex = lightStepIndex;
	}
}

void  effectMusicShader1::setAvgFreqLevel(float avg, float avgLow, float avgMid, float avgHigh)
{
	controlPanel->setValueF("avg", avg);
	controlPanel->setValueF("low", avgLow);
	controlPanel->setValueF("mid", avgMid);
	controlPanel->setValueF("high", avgHigh);

	if(avg > 0.8f)
		pulse(1);

	if(avgLow > 0.5f)
		pulse(2);

	if(avgMid > 0.5f)
		pulse(3);

	if(avgHigh > 0.5f)
		pulse(4);
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

	ofMatrix4x4 scaleMatrix;
	scaleMatrix.makeScaleMatrix(0.5f, 0.5f, 0.5f);
	ofMatrix4x4 centerTrans, recenterTrans;
	centerTrans.makeTranslationMatrix(-256.f, -256.f, 0.f);
	ofMatrix4x4 projectorRot;
	projectorRot.makeRotationMatrix(t * -60.0f, 0.0f, 0.0f, 1.0f); // rotate around projection axis
	ofMatrix4x4 projectorTilt;
	projectorTilt.makeRotationMatrix(-5.0f, 1.0f, 0.0f, 0.0f); // tilt over x axis
	ofMatrix4x4 projectorTrans;
	projectorTrans.makeTranslationMatrix(0.0f, 120.0f, 0.0f);

	//ofMatrix4x4 starsProjectorMatrix = projectorTrans * (projectorTilt * (projectorRot * centerTrans));
	ofMatrix4x4 starsProjectorMatrix =   scaleMatrix * centerTrans * projectorRot * projectorTilt * projectorTrans;

	switch(centralPatternMotion)
	{
	case 0:
		{
			scaleMatrix.makeScaleMatrix(0.6f, -0.6f, 0.6f);
			projectorRot.makeIdentityMatrix();
			break;
		}
	case 1: // thump
		{
			float scale = 0.4f + 0.1f * pulseFactors[0];
			scaleMatrix.makeScaleMatrix(scale, -scale, scale);
			projectorRot.makeIdentityMatrix();
			break;
		}
	case 2: // rotate
		{
			scaleMatrix.makeScaleMatrix(0.2f, 0.2f, 0.2f);
			projectorRot.makeRotationMatrix(t * 30.0f, 0.0f, 0.0f, 1.0f); 
			break;
		}
	}

	centerTrans.makeTranslationMatrix(-256.f, -256.f, 0.f);
	projectorTrans.makeTranslationMatrix(0.0f, 80.0f, 0.0f);
	//ofMatrix4x4 flowerProjectorMatrix = projectorTrans * (projectorRot * centerTrans);
	ofMatrix4x4 flowerProjectorMatrix = centerTrans * scaleMatrix * projectorRot * projectorTilt * projectorTrans;

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
	lightCol[0][0] = (0.5f + pulseFactors[2] * 0.5f) * mainLightsLevel;
	lightCol[0][1] = 0.0f;
	lightCol[0][2] = 0.0f;

	lightCol[1][0] = 0.0f;
	lightCol[1][1] = (0.5f + pulseFactors[3] * 0.5f) * mainLightsLevel;
	lightCol[1][2] = 0.0f;

	lightCol[2][0] = 0.0f;
	lightCol[2][1] = 0.0f;
	lightCol[2][2] = (0.5f + pulseFactors[4] * 0.5f) * mainLightsLevel;
	// ----------------------------------

	shader.begin();
	shader.setUniform1f("elapsedTime", t);
	shader.setUniform4fv("color", color);
	shader.setUniform3fv("lightPos", (float*)lightPos, 3);
	shader.setUniform3fv("lightCol", (float*)lightCol, 3);
	shader.setUniformTexture("pattern", *(centralPatternIndex == 0 ? picture1->getImagePtr() : heart->getImagePtr()), 1);
	shader.setUniformTexture("stars", *(stars->getImagePtr()), 2);
	shader.setUniform1f("starsFactor", pulseFactors[0]);
	shader.setUniform1f("flowerFactor", panel.getValueF("flowerFactor"));

	// for light bars mode
	shader.setUniform1f("lightBarLowY", lightBarLowY);
	shader.setUniform1f("lightBarHighY", lightBarHighY);
	

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

