#include "testApp.h"

void effectSingleTextureMap::SetFadeParameters(bool in, bool out, float time)
{
	fadeIn = in;
	fadeOut = out;
	fadeTime = time;
	textureFader.setup(fadeTime);
}

void effectSingleTextureMap::setupControlPanel(ofxAutoControlPanel& panel)
{
	controlPanel = &panel;

	showDefinition* show = showDefinition::getInstance();

	for(int i=0; i<show->movies.size(); i++)
	{
		// only add this movie to the GUI if the name is present in the list of selectedResourceNames
		if (std::find(selectedResourceNames.begin(), selectedResourceNames.end(), show->movies[i]->name) != selectedResourceNames.end())
		{
			panel.addToggle(show->movies[i]->makeGuiName(), false);
		}
	}

	for(int i=0; i<show->pictures.size(); i++)
	{
		// .. same for the pictures
		if (std::find(selectedResourceNames.begin(), selectedResourceNames.end(), show->pictures[i]->name) != selectedResourceNames.end())
		{
			panel.addToggle(show->pictures[i]->makeGuiName(), false);
		}
	}
}


void effectSingleTextureMap::update(ofxAutoControlPanel& panel)
{
	reloadShaderIfNeeded(panel);

	// TODO: get selected texture from control panel, update currentResource pointer
	// check the status of the GUI, change the texture if necessary
	// if a movie is playing, update that movie
	// if we switched movies, pause the one and resume/play the other
	updateCurrentTexture(panel);

}

// this function is a bit complicated because we are emulating a radiobutton with
// checkboxes. Two checkboxes can be selected at the same time.. but we want to have
// only one selection, and that the new selection takes precedence over the old one

showResource* effectSingleTextureMap::findNewSelectedResource(ofxAutoControlPanel& panel, bool& anySelectionMade)
{
	showDefinition* show = showDefinition::getInstance();

	anySelectionMade = false;
	showResource* newSelection = NULL;

	
	for(std::vector<string>::iterator nit = selectedResourceNames.begin(); nit != selectedResourceNames.end(); ++nit)
	{
		showResource* r = show->findResourceByName((*nit));
		if( r!= NULL)
			if(r->isSelected(panel))
			{
				anySelectionMade = true;
				if(r != currentResource )
				{
					newSelection = r;
				}
			}
	}

	return newSelection;
}

void effectSingleTextureMap::updateSelectedResourceCheckbox(ofxAutoControlPanel& panel)
{
	showDefinition* show = showDefinition::getInstance();

	for(std::vector<movieResource*>::iterator mit = show->movies.begin(); mit != show->movies.end(); ++mit)
	{
		if(nextResource == NULL)
			panel.setValueB((*mit)->makeGuiName(), false);
		else
		{
			if((*mit)->filePath == nextResource->filePath)
				panel.setValueB((*mit)->makeGuiName(), true);
			else
				panel.setValueB((*mit)->makeGuiName(), false);
		}
	}

	for(std::vector<pictureResource*>::iterator pit = show->pictures.begin(); pit != show->pictures.end(); ++pit) 
	{
		if(nextResource == NULL)
			panel.setValueB((*pit)->makeGuiName(), false);
		else
		{
			if((*pit)->filePath == nextResource->filePath)
				panel.setValueB((*pit)->makeGuiName(), true);
			else
				panel.setValueB((*pit)->makeGuiName(), false);
		}
	}
}

// note: this is rather convoluted.. and all because we wanted to be able to stop/pause
// a video by not selecting a checkbox. Otherwise we could have used a radio button
// Now it turns out that pausing a video doesn't work, because the logic of the app
// dictates that nothing will be rendered if nothing is selected..
// So better would be to replace this with a simple radiobutton and some video-player
// controls to start, pause, rewind (etc) the video
void effectSingleTextureMap::updateCurrentTexture(ofxAutoControlPanel& panel)
{
	// update the fader if we are in that state
	if(fadeState == FadeState::FadingIn || fadeState == FadeState::FadingOut)
	{
		textureFader.update();

		// we always complete a fade before switching to a new state
		if(textureFader.isFading())
		{
			cout << "Waiting until fade is done, fadeState=" << fadeState << endl;
		}
		else
		{
			// apparently we *just* stopped fading, so complete the action
			if(fadeState == FadeState::FadingOut)
			{
				currentResource->goOutOfView();
				SwitchState(FadeState::Idle);

				// if a next resource is already in waiting, set that pointer
				// and fade it in (if fadeIn was set)
				currentResource = nextResource;

				if(currentResource != NULL)
				{
					if(fadeIn)
					{
						cout << "Automatically starting fade-in after fade-out is complete" << endl;
						SwitchState(FadeState::FadingIn);
					}
					currentResource->comeIntoView();
				}
			}
			else
			{
				// fade in or out is done
				SwitchState(FadeState::Idle);
			}
		}
	}

	if(!textureFader.isFading())
	{
		//showResource* prevResource = currentResource;

		bool anySelectionMade = false;
		showResource* newSelection = findNewSelectedResource(panel, anySelectionMade);


		if(anySelectionMade == false) // no selection was made
			nextResource = NULL;
		else
		{
			if(newSelection != NULL) // there was a selection, and it's new
				nextResource = newSelection;
			else // there was a selection, but it's the same as before
			{
				// keep current resource as it is
			}
		}

		// find selected resource
		// if different from what we had 
		if(currentResource != nextResource)
		{
			if(fadeOut && currentResource != NULL)
			{
				SwitchState(FadeState::FadingOut);

				// If we need to fade out and also in, then the fade in
				// will be delayed until the fade out is done. This is handled
				// at the top of this function.
				// 'nextResource' has already been set properly, we keep 'currentResource' as it is
			}
			else
			{
				if(currentResource != NULL)
					currentResource->goOutOfView();

				currentResource = nextResource;

				if(fadeIn)
				{
					SwitchState(FadeState::FadingIn);
				}
				else
				{
					// no fading, switch immediately
					SwitchState(FadeState::Idle);
				}

				if(currentResource != NULL)
				{
					currentResource->comeIntoView();
				}
			}
		}
	}
	updateSelectedResourceCheckbox(panel);

	// also while fading we need to call this, so outside of the if(!fading) block
	if(currentResource != NULL)
	{
		//cout << "Updating currentResource" << endl;
		currentResource->update();

		// try to 'warm up' the GPU state by loading the texture now instead of in render
		getCurrentTexture();
	}
}


void effectSingleTextureMap::SwitchState(FadeState newState)
{
	if(newState == fadeState) // no change
		return;

	cout << "SwitchState() newState=" << newState << endl;

	if(newState == FadeState::FadingIn)
	{
		textureFader.fadeIn();
	}
	else if(newState == FadeState::FadingOut)
	{
		textureFader.fadeOut();
	}
	fadeState = newState;
}

ofTexture* effectSingleTextureMap::getCurrentTexture()
{
	if(currentResource != NULL)
		return currentResource->getTexturePtr();
	else
		return NULL;
}

void effectSingleTextureMap::stop()
{
	if(currentResource != NULL)
		currentResource->goOutOfView();

	currentResource = NULL;

	updateSelectedResourceCheckbox(*controlPanel);
}

void effectSingleTextureMap::render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh)
{
	ofPushStyle();

/*	if(panel.getValueB("highlight")) {
		mesh->clearColors();
		int n = mesh->getNumVertices();
		float highlightPosition = panel.getValueF("highlightPosition");
		float highlightOffset = panel.getValueF("highlightOffset");
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			mesh->addColor(cur);
		}
	}*/
	
	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);

	// set up shader

	if(currentResource != NULL)
	{
		shader.begin();

		float fadeFactor = 1;
		if(fadeState == FadeState::FadingIn || fadeState == FadeState::FadingOut)
		{
			fadeFactor = textureFader.getCurrentValue();
			cout << "fadeFactor=" << fadeFactor << endl;
		}

		shader.setUniform1f("fadeFactor", fadeFactor);
		shader.setUniformTexture("background", *currentResource->getTexturePtr(), 1);
		shader.end();

		shader.begin();

		//ofTexture& pictureTexture = customPicture.getTextureReference();
		//pictureTexture.bind();
		//drawModel(OF_MESH_FILL, &pictureTexture);
		//pictureTexture.unbind();
		//currentTexture->bind();
		drawModel(OF_MESH_FILL, model);
		//currentTexture->unbind();

		shader.end();
	}
//	else
//		cout << "Cannot render, currentResource is NULL" << endl;

	glPopAttrib();

	ofPopStyle();

}

void effectSingleTextureMap::drawModel(ofPolyRenderMode renderType, ofxAssimpModelLoader* model)
{
	ofPushStyle();
    

	if(!ofGetGLProgrammableRenderer()){
	#ifndef TARGET_OPENGLES
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPolygonMode(GL_FRONT_AND_BACK, ofGetGLPolyMode(renderType));
	#endif
		glEnable(GL_NORMALIZE);
	}
    
	ofEnableNormalizedTexCoords();

	ofPushMatrix();
	//ofMultMatrix(modelMatrix);
    
	for(unsigned int i=0; i<model->getNumMeshes(); i++) {
		ofxAssimpMeshHelper & mesh = model->getMeshHelper(i);
        
		ofPushMatrix();
		//ofMultMatrix(mesh->matrix);

		// if no texture override is specified then try to use the texture from the mesh definition
		if(currentResource == NULL)
		{
			//if(model.bUsingTextures){
				if(mesh.hasTexture()) {
					ofTexture * tex = mesh.getTexturePtr();
					if(tex->isAllocated()) {
						tex->bind();
					}
				}
			//}
		}

		//if(bUsingMaterials){
			mesh.material.begin();
		//}
        
		if(mesh.twoSided) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
        
		ofEnableBlendMode(mesh.blendMode);
#ifndef TARGET_OPENGLES
		mesh.vbo.drawElements(GL_TRIANGLES,mesh.indices.size());
#else
		switch(renderType){
			case OF_MESH_FILL:
		    	mesh->vbo.drawElements(GL_TRIANGLES,mesh->indices.size());
		    	break;
			case OF_MESH_WIREFRAME:
		    	mesh->vbo.drawElements(GL_LINES,mesh->indices.size());
		    	break;
			case OF_MESH_POINTS:
		    	mesh->vbo.drawElements(GL_POINTS,mesh->indices.size());
		    	break;
		}
#endif

		if(currentResource == NULL)
		{
			//if(bUsingTextures){
				if(mesh.hasTexture()) {
					ofTexture * tex = mesh.getTexturePtr();
					if(tex->isAllocated()) {
						tex->unbind();
					}
				}
			//}
		}

		//if(bUsingMaterials){
			mesh.material.end();
		//}
        
		ofPopMatrix();
	}
    
	ofPopMatrix();

	if(!ofGetGLProgrammableRenderer()){
	#ifndef TARGET_OPENGLES
		glPopClientAttrib();
		glPopAttrib();
	#endif
	}
	ofPopStyle();
}

