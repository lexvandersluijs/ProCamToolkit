#version 130
#extension GL_ARB_texture_rectangle : enable

#define PI (3.1415926536)
#define TWO_PI (6.2831853072)

uniform sampler2DRect background;
uniform sampler2DRect overlay;
uniform sampler2DRect wavefront;

uniform float backgroundFactor = 1.0;
uniform float overlayFactor = 1.0;

uniform float elapsedTime;

varying vec3 position, normal;
varying float randomOffset;

const vec4 on = vec4(1.0);
const vec4 off = vec4(vec3(0.), 1.);
const vec4 yellow = vec4(253./255., 254./255., 145./255., 1.);
const vec4 black = vec4(0., 0., 0., 1.);

void main() 
{
	vec4 c = vec4(0.0, 0.0, 0.0, 1.0);

	vec2 pos = gl_TexCoord[0].st * 2048.0;

	// simply copy background:
	//c = vec4(texture2DRect( background, pos ).rgb * backgroundFactor, 1.0);
	vec4 bgColor = vec4(texture2DRect( background, pos ).rgb * backgroundFactor, 1.0);

	vec2 posOverlay = gl_TexCoord[0].st * 2048.0;
	vec4 overlayColor = vec4(texture2DRect( overlay, posOverlay ).rgb * overlayFactor, 1.0);


	// fade in the background starting from the bottom up
	float sequenceDuration = 20.0;
	float sequenceFraction = mod(elapsedTime, sequenceDuration) / sequenceDuration;

	if(sequenceFraction < 0.66666)
	{
		float fadeUpFraction = sequenceFraction / 0.66666; //mod(elapsedTime, fadeDuration) / fadeDuration;
		float frontHeight = fadeUpFraction * 190.0;
		float frontSize = 19; // 1 stair for the wavefront size?

		if(position.y > frontHeight)
		{
			// stays black
		}
		else
		{
			if((position.y < frontHeight) && (position.y > (frontHeight - frontSize)))
			{
				float relativePos = position.y - (frontHeight-frontSize);

				if(relativePos > 16 && relativePos < 18)
				{
					c = on; // white scanline
				}
				else
				{
					float wavefrontFactor = relativePos / frontSize;
					vec4 wavefrontColor = texture2DRect( wavefront, vec2(32, 64 * (1.0 - wavefrontFactor) ));
					c = vec4((bgColor.r + overlayColor.r) * wavefrontColor.rgb, 1.0);
				}

			}
			else
			{
				// below the wavefront: 100% of texmap value, at full opacity
				c = vec4((bgColor.r + overlayColor.r) * yellow.rgb, 1.);
			}
		}
	}
	else
	{
		if(sequenceFraction < 0.8) // short stationary time
		{
			c = vec4((bgColor.r + overlayColor.r) * yellow.rgb, 1.);
		}
		else
		{
			if(sequenceFraction < 0.9) // fadeout
			{
				float fadeOutFraction = (sequenceFraction - 0.8) / 0.1;
				c = vec4((bgColor.r + overlayColor.r) * yellow.rgb * (1. - fadeOutFraction), 1.);
			}
			else // black - short pause until we fade up again
			{
				c = black;
			}
		}
	}

	gl_FragColor = c;
}
