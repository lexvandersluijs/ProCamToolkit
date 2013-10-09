#pragma once

class pictureResource;

// this class is a bit of a hack
// created under time pressure..
// this class hierarchy / effect system should be thought out more thoroughly
class effectMultiTextureShader : public effectSingleTextureMap
{
public:
	effectMultiTextureShader();
	void setup();
	void update(ofxAutoControlPanel& panel);
	void render(ofxAutoControlPanel& panel, ofxAssimpModelLoader* model, ofMesh* mesh);

protected:
	pictureResource* background;	
	pictureResource* wavefront;	
};
