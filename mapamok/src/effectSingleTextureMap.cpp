#include "testApp.h"


void effectSingleTextureMap::setupControlPanel(ofxAutoControlPanel& panel)
{
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
		if (std::find(selectedResourceNames.begin(), selectedResourceNames.end(), show->movies[i]->name) != selectedResourceNames.end())
		{
			panel.addToggle(show->pictures[i]->makeGuiName(), false);
		}
	}
}


void effectSingleTextureMap::update(ofxAutoControlPanel& panel)
{
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

	for(std::vector<movieResource*>::iterator mit = show->movies.begin(); mit != show->movies.end(); ++mit)
	{
		if((*mit)->isSelected(panel))
		{
			anySelectionMade = true;
			if((*mit) != currentResource )
			{
				newSelection = *mit;
			}
		}
	}

	for(std::vector<pictureResource*>::iterator pit = show->pictures.begin(); pit != show->pictures.end(); ++pit) 
	{
		if((*pit)->isSelected(panel))
		{
			anySelectionMade = true;
			if((*pit) != currentResource)
			{
				newSelection = *pit;
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
		if(currentResource == NULL)
			panel.setValueB((*mit)->makeGuiName(), false);
		else
		{
			if((*mit)->filePath == currentResource->filePath)
				panel.setValueB((*mit)->makeGuiName(), true);
			else
				panel.setValueB((*mit)->makeGuiName(), false);
		}
	}

	for(std::vector<pictureResource*>::iterator pit = show->pictures.begin(); pit != show->pictures.end(); ++pit) 
	{
		if(currentResource == NULL)
			panel.setValueB((*pit)->makeGuiName(), false);
		else
		{
			if((*pit)->filePath == currentResource->filePath)
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
	showResource* prevResource = currentResource;

	bool anySelectionMade = false;
	showResource* newSelection = findNewSelectedResource(panel, anySelectionMade);


	if(anySelectionMade == false) // no selection was made
		currentResource = NULL;
	else
	{
		if(newSelection != NULL) // there was a selection, and it's new
			currentResource = newSelection;
		else // there was a selection, but it's the same as before
		{
			// keep current resource as it is
		}
	}

	// find selected resource
	// if different from what we had 
	if(prevResource != currentResource)
	{
		//   disable the toggle of the previous one
		if(prevResource != NULL)
		{
			prevResource->goOutOfView();
		}

		if(currentResource != NULL)
		{
			currentResource->comeIntoView();
		}
	}

	updateSelectedResourceCheckbox(panel);

	if(currentResource != NULL)
	{
		currentResource->update();
	}

	currentTexture = getCurrentTexture();
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

	if(currentTexture != NULL)
	{
		//ofTexture& pictureTexture = customPicture.getTextureReference();
		//pictureTexture.bind();
		//drawModel(OF_MESH_FILL, &pictureTexture);
		//pictureTexture.unbind();
		currentTexture->bind();
		drawModel(OF_MESH_FILL, model);
		currentTexture->unbind();
	}

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
		if(currentTexture == NULL)
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

		if(currentTexture == NULL)
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

