#include "testApp.h"
#include "Poco/Util/XMLConfiguration.h"

using namespace ofxCv;
using namespace cv;

showDefinition* showDefinition::instance;

// --------------------- showResource ------------------

bool showResource::isSelected(ofxControlPanel& panel)
{
	if(panel.getValueB(makeGuiName()))
		return true;
	else
		return false;
}

// ---------------------- movieResource ------------------
movieResource::~movieResource()
{
	stopCloseDeleteMovie();
}

void movieResource::stopCloseDeleteMovie()
{
	if(movie != NULL)
	{
		if(movie->isPlaying() || movie->isPaused())
			movie->stop();
		movie->close();
		delete movie;
		movie = NULL;
	}

	if(player != NULL)
	{
		if(player->isPlaying() || player->isPaused())
			player->stop();
		player->close();
		delete player;
		player = NULL;
	}
}
void movieResource::load()
{
	//movie = new ofxThreadedVideo();
	//movie->loadMovie(filePath);
	//movie->setLoopState(OF_LOOP_NORMAL);
}
ofTexture* movieResource::getTexturePtr()
{ 
	//return &movie->getTextureReference(); 
	return player->getGstVideoUtils()->getTexturePtr();
}

void movieResource::comeIntoView()
{
	ofLogNotice("mapamok") << "movie " << name << " comes into view";


	bool useThreadedVideoPlayer = false;
	if(useThreadedVideoPlayer)
	{
		if (movie != NULL)
		{
			ofLogError("mapamok") << "movieResource::comeIntoView, movie should be NULL but it is not!";
			return;
		}

		movie = new ofxThreadedVideo();
		movie->loadMovie(filePath);
		movie->setLoopState(OF_LOOP_NORMAL);
		movie->play();
	}
	else // use gstreamer
	{
		if (player != NULL)
		{
			ofLogError("mapamok") << "movieResource::comeIntoView, player should be NULL but it is not!";
			return;
		}
		player = new ofxGstStandaloneVideoPlayer();

		std::string file = ofFilePath::getAbsolutePath(filePath, true);
		std::replace(file.begin(), file.end(), '\\', '/');
		file = "file:///" + file;

		player->setPixelFormat(OF_PIXELS_RGB);
		player->loadMovie(file);
		player->setLoopState(OF_LOOP_NORMAL);
		player->play();
	}
}

void movieResource::goOutOfView()
{
	ofLogNotice("mapamok") << "movie " << name << " goes out of view";

	if(movie != NULL)
	{
		if(movie->isPlaying() == true)
		{
			movie->stop();
		}

		stopCloseDeleteMovie();
	}

	if(player != NULL)
	{
		if(player->isPlaying())
		{
			player->stop();
		}
		stopCloseDeleteMovie();
	}
}
void movieResource::update()
{
	if(movie != NULL)
		if(movie->isPlaying())
			movie->update();

	if(player != NULL)
		if(player->isPlaying())
			player->update();
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
showDefinition::showDefinition()
{
	currentSegmentIndex = -1;
	currentSegment = NULL;

	instance = this;
}
showDefinition::~showDefinition()
{
	// note: all resources are freed in end() function, so this must be called before the program exits!
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

void showDefinition::setup()
{
	// some resource types probably need an OpenGL context, so we do this here
	// not in main where the XML was read
	loadResources();


	for(std::vector<showSegment*>::iterator sit = showSegments.begin(); sit != showSegments.end(); ++sit)
	{
		(*sit)->setup();
	}
}

void showDefinition::update(mainControlPanel& panel)
{

	if(currentSegment != NULL)
		currentSegment->update();
}

void showDefinition::end()
{
	for(std::vector<showSegment*>::iterator sit = showSegments.begin(); sit != showSegments.end(); ++sit)
	{
		(*sit)->end();
	}

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

pictureResource* showDefinition::findPictureByName(string name)
{
	pictureResource* result = NULL;

	for(std::vector<pictureResource*>::iterator pit = pictures.begin(); pit != pictures.end(); ++pit) 
	{
		if ( (*pit)->name == name )
		{
			result = (*pit);
			break;
		}
	}

	return result;
}

showResource* showDefinition::findResourceByName(string name)
{
	showResource* result = NULL;

	for(std::vector<pictureResource*>::iterator pit = pictures.begin(); pit != pictures.end(); ++pit) 
	{
		if ( (*pit)->name == name )
		{
			result = (*pit);
			break;
		}
	}
	for(std::vector<movieResource*>::iterator mit = movies.begin(); mit != movies.end(); ++mit) 
	{
		if ( (*mit)->name == name )
		{
			result = (*mit);
			break;
		}
	}

	return result;
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
	
	// ----------- get the 3D model location and set it ------------
	Poco::XML::Node* modelNode = pDoc->getNodeByPath("/show/model");
	if(modelNode != NULL)
	{
		Poco::XML::NamedNodeMap* modelNodeAttribs = modelNode->attributes();
		for(int i=0; i<modelNodeAttribs->length(); i++)
		{
			Poco::XML::Node* attribute = NULL;
			attribute = modelNodeAttribs->item(i);

			if(attribute->nodeName() == "file")
				modelLocation = attribute->nodeValue();
		}
	}
	else
		cout << "Error: no 3D model was defined in the show definition Xml file" << endl;

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


showDefinition* showDefinition::getInstance()
{
	return instance;
}

void showDefinition::setCurrentSegment(int segmentIndex)
{
	if(segmentIndex >= 0 && segmentIndex < showSegments.size())
	{
		currentSegmentIndex = segmentIndex;
		currentSegment = showSegments[segmentIndex];
	}
	else
		currentSegment = NULL;
}