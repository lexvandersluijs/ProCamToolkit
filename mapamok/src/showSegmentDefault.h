#pragma once

class showSegmentDefault : public showSegment
{
private:
	ofShader shader;
	ofLight light;


public:
	showSegmentDefault();

	virtual void setup();
	virtual void update();
	virtual void render();

};
