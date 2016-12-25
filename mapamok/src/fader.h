#pragma once

#include <algorithm>

class fader
{
public:
	fader()
	{
		fadeStartTime = 0.f;
		fadeDuration = 1.f;
		currentValue = 0.f;
		in = false;
		smooth = true;
		fading = false; // fading and fadeComplete are redundant..
		fadeComplete = true;
	}

	void setup(float duration)
	{
		fadeDuration = duration;
	}
	void fadeIn()
	{
		fadeStartTime = ofGetElapsedTimef();
		in = true;
		fading = true;
		fadeComplete = false;
	}
	void fadeOut()
	{
		fadeStartTime = ofGetElapsedTimef();
		in = false;
		fading = true;
		fadeComplete = false;
	}
	bool isFading()
	{
		return fading;
	}
	void update();
	
	bool canFadeIn()
	{
		return (fading == false) && (fadeComplete == true) && (in == false);
	}
	bool canFadeOut()
	{
		return (fading == false) && (fadeComplete == true) && (in == true);
	}
	float getCurrentValue()
	{
		return currentValue;
	}

protected:
	float fadeStartTime;
	float fadeDuration;
	float currentValue;
	bool in;
	bool smooth;
	bool fading;
	bool fadeComplete;
};
