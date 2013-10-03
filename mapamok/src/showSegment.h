#pragma once

class showSegment
{
public:
	// the name of this segment, for clarity in the UI, and for persistence in XML
	string name;

	virtual void setup() { }
	virtual void start() { }
	virtual void update() { }
	virtual void draw() { }
	virtual void end() { }
};
