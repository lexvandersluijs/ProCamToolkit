#include "testApp.h"

float clamp(float val, float minVal, float maxVal)
{
	return std::max(std::min(val, maxVal), minVal);
}
float smoothstep(float edge0, float edge1, float x)
{
	// Scale, bias and saturate x to 0..1 range
	x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0); 
	// Evaluate polynomial
	return x*x*(3 - 2*x);
}

void fader::update()
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
					if(smooth)
					{
						currentValue = smoothstep(0.f, 1.f, fraction);
					}
					else
					{
						currentValue = fraction;
					}
				}
				else // fade out
				{
					if(smooth)
						currentValue = 1.f - smoothstep(0.f, 1.f, fraction);
					else
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

