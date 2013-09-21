#include "testApp.h"
#include "Poco/Util/XMLConfiguration.h"

using namespace ofxCv;
using namespace cv;

showDefinition::~showDefinition()
{
	vector<showResource*>::iterator it = movies.begin();
	while (it != movies.end()) 
	{
		delete *it;
		it = movies.erase(it);    
	}

	it = pictures.begin();
	while (it != pictures.end()) 
	{
		delete *it;
		it = pictures.erase(it);    
	}
}

void showDefinition::addMovie(string filePath, string name)
{
	showResource* sr = new showResource();
	sr->filePath = filePath;
	sr->name = name;
	movies.push_back(sr);
}

void showDefinition::addPicture(string filePath, string name)
{
	showResource* sr = new showResource();
	sr->filePath = filePath;
	sr->name = name;
	pictures.push_back(sr);
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