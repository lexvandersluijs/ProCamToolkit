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
	float fadeDuration = 10.0;
	float sequenceDuration = 15.0;
	float sequenceFraction = mod(elapsedTime, sequenceDuration) / sequenceDuration;

	
	float fadeUpFraction = mod(elapsedTime, fadeDuration) / fadeDuration;
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
				c = on;
			}
			else
			{
				float wavefrontFactor = relativePos / frontSize;
				vec4 wavefrontColor = texture2DRect( wavefront, vec2(32, 64 * (1.0 - wavefrontFactor) ));
				c = vec4((bgColor.rgb + overlayColor.rgb) * wavefrontColor.r, 1.0);
			}

		}
		else
		{
			// below the wavefront: 100% of texmap value, at full opacity
			c = vec4(bgColor.rgb + overlayColor.rgb, 1.0);
		}
	}


	gl_FragColor = c;
}
