#pragma once

class showSegmentDefault : public showSegment
{
private:
	ofShader shader;
	ofLight light;


public:
	virtual void setup();
	virtual void update();
	virtual void draw();

};
