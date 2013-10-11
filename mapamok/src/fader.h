#pragma once

class fader
{
public:
	fader()
	{
		fadeStartTime = 0.f;
		fadeDuration = 1.f;
		currentValue = 0.f;
		in = false;
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
	void update()
	{
		if(fading)
		{
			float t = ofGetElapsedTimef();
			float dif = t - fadeStartTime;

			if( dif >= 0 )
			{
				if(dif < fadeDuration)
				{
					float fraction = dif / fadeDuration;
					if(in)
					{
						// for now: just linear interp..
						currentValue = fraction;
					}
					else // fade out
					{
						currentValue = 1.f - fraction;
					}
				}
				else
				{
					fading = false;
					fadeComplete = true;
					if(in)
					{
						currentValue = 1.f;
					}
					else
					{
						currentValue = 0.f;
					}
				}
			}
		}
	}

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
	bool fading;
	bool fadeComplete;
};
