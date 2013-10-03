#include "testApp.h"
#include "Poco/Util/XMLConfiguration.h"

using namespace ofxCv;
using namespace cv;

// --------------------- showResource ------------------

bool showResource::isSelected(mainControlPanel& panel)
{
	if(panel.getValueB(makeGuiName()))
		return true;
	else
		return false;
}

// ---------------------- movieResource ------------------
movieResource::~movieResource()
{
	if(movie != NULL)
	{
		if(movie->isPlaying() || movie->isPaused())
			movie->stop();
		movie->close();
		delete movie;
	}
}
void movieResource::load()
{
	movie = new ofxThreadedVideo();
	movie->loadMovie(filePath);
}
ofTexture* movieResource::getTexturePtr()
{ 
	return &movie->getTextureReference(); 
}

void movieResource::comeIntoView()
{
	if(movie->isPlaying() == false)
		movie->play();
	else
		if(movie->isPaused())
			movie->setPaused(false); 
}

void movieResource::goOutOfView()
{
	if(movie->isPlaying() == true)
	{
		movie->setPaused(true); //.stop();
	}
}
void movieResource::update()
{
	if(movie->isPlaying())
		movie->update();
}
// ----------------------- pictureResource ------------------
void pictureResource::load()
{
	picture = new ofImage();
	picture->loadImage(filePath);
}
ofTexture* pictureResource::getTexturePtr()
{ 
	return &picture->getTextureReference(); 
}


// ---------------------------- showDefinition ----------------------
showDefinition::~showDefinition()
{
	vector<movieResource*>::iterator mit = movies.begin();
	while (mit != movies.end()) 
	{
		delete *mit;
		mit = movies.erase(mit);    
	}

	vector<pictureResource*>::iterator pit = pictures.begin();
	while (pit != pictures.end()) 
	{
		delete *pit;
		pit = pictures.erase(pit);    
	}
}

void showDefinition::addMovie(string filePath, string name)
{
	movieResource* sr = new movieResource();
	sr->filePath = filePath;
	sr->name = name;
	movies.push_back(sr);
}

void showDefinition::addPicture(string filePath, string name)
{
	pictureResource* sr = new pictureResource();
	sr->filePath = filePath;
	sr->name = name;
	pictures.push_back(sr);
}

void showDefinition::loadResources()
{
	for(std::vector<movieResource*>::iterator mit = movies.begin(); mit != movies.end(); ++mit)
	{
		(*mit)->load();
	}
	for(std::vector<pictureResource*>::iterator pit = pictures.begin(); pit != pictures.end(); ++pit) 
	{
		(*pit)->load();
	}

}

void showDefinition::load(string configName)
{

	// from that folder, load projectorConfiguration.xml
	// read its values and set up the projConfig structure. 
	// .. while doing this, the name of the projector that is associated with a viewport is also set
	// .. from this name, the projector will know where to find its calibration data
	// .. then: let the projConfig load its calibration
	// .. each projector will check if its folder exists, and then load

	string configFolder = ofToDataPath(configName);
	string configFilePath = configFolder + "\\show.xml";
	ifstream configFile(configFilePath);
			
	// this works, but we can't enumerate multiple elements of the same type..
	//AutoPtr<XMLConfiguration> pConf = new XMLConfiguration(configFile);
	//string width = pConf->getString("viewports.projectorView[@name='Top Left'][@width]");

	Poco::XML::InputSource src(configFile);
	Poco::XML::DOMParser parser;
	Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
			
	// -------------- add all movies -------------
	Poco::XML::Node* moviesNode = pDoc->getNodeByPath("/show/movies");
	Poco::XML::NodeList* movList = moviesNode->childNodes();
	for(int currentMovieIndex=0; currentMovieIndex<movList->length(); currentMovieIndex++)
	{
		Poco::XML::Node* movNode = movList->item(currentMovieIndex);
		if(movNode->nodeType() == 1)
		{
			Poco::XML::NamedNodeMap* attribs = movNode->attributes();
			string file, name;
			for(int i=0; i<attribs->length(); i++)
			{
				Poco::XML::Node* attribute = NULL;
				attribute = attribs->item(i);

				if(attribute->nodeName() == "file")
					file = attribute->nodeValue();
				else if (attribute->nodeName() == "name")
					name = attribute->nodeValue();
			}
			addMovie(file, name);
		}
	}

	// -------------- add all pictures -------------
	Poco::XML::Node* picturesNode = pDoc->getNodeByPath("/show/pictures");
	Poco::XML::NodeList* picList = picturesNode->childNodes();
	for(int currentPicIndex=0; currentPicIndex<picList->length(); currentPicIndex++)
	{
		Poco::XML::Node* picNode = picList->item(currentPicIndex);
		if(picNode->nodeType() == 1)
		{
			Poco::XML::NamedNodeMap* attribs = picNode->attributes();
			string file, name;
			for(int i=0; i<attribs->length(); i++)
			{
				Poco::XML::Node* attribute = NULL;
				attribute = attribs->item(i);

				if(attribute->nodeName() == "file")
					file = attribute->nodeValue();
				else if (attribute->nodeName() == "name")
					name = attribute->nodeValue();
			}
			addPicture(file, name);
		}
	}

}

// this function is a bit complicated because we are emulating a radiobutton with
// checkboxes. Two checkboxes can be selected at the same time.. but we want to have
// only one selection, and that the new selection takes precedence over the old one

showResource* showDefinition::findNewSelectedResource(mainControlPanel& panel, bool& anySelectionMade)
{
	anySelectionMade = false;
	showResource* newSelection = NULL;

	for(std::vector<movieResource*>::iterator mit = movies.begin(); mit != movies.end(); ++mit)
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

	for(std::vector<pictureResource*>::iterator pit = pictures.begin(); pit != pictures.end(); ++pit) 
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

void showDefinition::updateSelectedResourceCheckbox(mainControlPanel& panel)
{
	for(std::vector<movieResource*>::iterator mit = movies.begin(); mit != movies.end(); ++mit)
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

	for(std::vector<pictureResource*>::iterator pit = pictures.begin(); pit != pictures.end(); ++pit) 
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
void showDefinition::updateCurrentTexture(mainControlPanel& panel)
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

}

ofTexture* showDefinition::getCurrentTexture()
{
	if(currentResource != NULL)
		return currentResource->getTexturePtr();
	else
		return NULL;
}