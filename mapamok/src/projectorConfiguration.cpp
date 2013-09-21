#include "testApp.h"
#include "Poco/Util/XMLConfiguration.h"

using namespace ofxCv;
using namespace cv;


void projectorConfiguration::saveCalibration()
{
	// create a base name for the calibration file(s), based on the name of this configuration
	string dirName = name + "/";
	ofDirectory dir(dirName); // this is relative to the current working folder, apparently set to <programfolder>\data by OF
	dir.create();

	// pass this on to each projector and have it save its own data
	// each projector will check if this file already exists, and overwrite it if so
	for(int i=0; i<nrOfProjViews; i++)
		projViews[i].proj.saveCalibration(dirName);
}

void projectorConfiguration::loadCalibration()
{
	// create a base name for the calibration file(s), based on the name of this configuration
	string dirName = name + "/";

	// pass this on to each projector and have it save its own data
	for(int i=0; i<nrOfProjViews; i++)
		projViews[i].proj.loadCalibration_fromConfigFolder(dirName);

}

void projectorConfiguration::loadConfiguration(string configName)
{
	setName(configName);

	// from that folder, load projectorConfiguration.xml
	// read its values and set up the projConfig structure. 
	// .. while doing this, the name of the projector that is associated with a viewport is also set
	// .. from this name, the projector will know where to find its calibration data
	// .. then: let the projConfig load its calibration
	// .. each projector will check if its folder exists, and then load

	string configFolder = ofToDataPath(configName);
	string configFilePath = configFolder + "\\projectorConfiguration.xml";
	ifstream configFile(configFilePath);
			
	// this works, but we can't enumerate multiple elements of the same type..
	//AutoPtr<XMLConfiguration> pConf = new XMLConfiguration(configFile);
	//string width = pConf->getString("viewports.projectorView[@name='Top Left'][@width]");

	Poco::XML::InputSource src(configFile);
	Poco::XML::DOMParser parser;
	Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
			
	// -------------- first count the number of viewports -------------
	int nrOfViewports = 0;
	Poco::XML::Node* viewportsNode = pDoc->getNodeByPath("/projectorConfiguration/viewports");
	Poco::XML::NodeList* vpList = viewportsNode->childNodes();
	for(int currentVpIndex=0; currentVpIndex<vpList->length(); currentVpIndex++)
	{
		Poco::XML::Node* vpNode = vpList->item(currentVpIndex);
		if(vpNode->nodeType() == 1)
		{
			++nrOfViewports;
		}
	}
	// ----------------- then allocate space and read + fill their properties ---------
	// also see http://stackoverflow.com/questions/15626048/how-can-i-use-poco-to-parse-an-xml-file-and-extract-a-particular-node-to-a-std
	initialize(nrOfViewports);
	int currentVpIndex = 0;
	for(int itemIndex=0; itemIndex<vpList->length(); itemIndex++)
	{
		Poco::XML::Node* vpNode = vpList->item(itemIndex);
		if(vpNode->nodeType() == 1)
		{
			projectorView* projView = getProjViewPtr(currentVpIndex);

			Poco::XML::NamedNodeMap* attribs = vpNode->attributes();
			string xs, ys, ws, hs, nm;
			for(int i=0; i<attribs->length(); i++)
			{
				Poco::XML::Node* attribute = NULL;
				attribute = attribs->item(i);

				if(attribute->nodeName() == "x")
					xs = attribute->nodeValue();
				else if (attribute->nodeName() == "y")
					ys = attribute->nodeValue();
				else if (attribute->nodeName() == "width")
					ws = attribute->nodeValue();
				else if (attribute->nodeName() == "height")
					hs = attribute->nodeValue();
				else if (attribute->nodeName() == "name")
					nm = attribute->nodeValue();
			}
			float x=::atof(xs.c_str());
			float y=::atof(ys.c_str());
			float width=::atof(ws.c_str());
			float height=::atof(hs.c_str());
			projView->setViewport(x, y, width, height); 
			projView->proj.name = nm;

			attribs->release();
			++currentVpIndex;
		}
	}
}