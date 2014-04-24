#version 130
#extension GL_ARB_texture_rectangle : enable

#define PI (3.1415926536)
#define TWO_PI (6.2831853072)

uniform sampler2DRect pattern;
uniform sampler2DRect stars;
uniform float starsFactor;
uniform float flowerFactor;

uniform float elapsedTime;
uniform vec4 color;

uniform vec3 lightPos[3];
uniform vec3 lightCol[3];
const int nrOfLights = 3;

uniform mat4 starsProjectorMatrix;
uniform mat4 flowerProjectorMatrix;

uniform float lightBarLowY = 0;
uniform float lightBarHighY = 0;

uniform int lightMode = 0;

varying vec3 position, normal;
varying float randomOffset;

const vec4 on = vec4(1.0);
const vec4 off = vec4(vec3(0.), 1.);

void main() 
{

	vec4 c = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 transformedFlowerPos = flowerProjectorMatrix * vec4(position, 1.0);
	vec2 flowerPos;
	flowerPos.x = mod(transformedFlowerPos.x, 512.0);
	flowerPos.y = mod(transformedFlowerPos.y, 512.0);

	c = vec4(texture2DRect( pattern, flowerPos ).rgb * flowerFactor, 1.0);

	if(lightMode == 0)
	{
		// using the normal texture coordinates
		//c += vec4(texture2DRect( stars, pos ).rgb * starsFactor, 1.0);

		if(abs(normal.x) < 0.8) // don't show light (stretched) on sides of stairs
		{
			// using our projector
			vec4 transformedPos = starsProjectorMatrix * vec4(position, 1.0);
			vec2 pos;
			pos.x = mod(transformedPos.x, 512.0);
			pos.y = mod(transformedPos.y, 512.0);

			c += vec4(texture2DRect( stars, pos ).rgb * starsFactor, 1.0);
		}
	}
	else 
	{
		if (lightMode == 1)
		{
			const float speed = -19.;
			const float scale = 19.;
			//c += (mod((position.x + position.y + position.z) + (elapsedTime * speed), scale) > scale / 2.) ? (on * starsFactor): off;
			c += (mod((position.y) + (elapsedTime * speed), scale) > scale / 2.) ? on : off;
		}
		else  // if (lightMode == 2)
		{
			if (position.y > lightBarLowY && position.y < lightBarHighY)
				c = on;
			else
				c = off;
		}
	}

	vec3 posToEye = normalize( -position.xyz );

	vec3 lightsContribution = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nrOfLights; i++)
	{
		vec3 posToLight = lightPos[i] - position;
		posToLight = normalize(posToLight);
		float eyeFactor = dot(posToEye, normal);
		float diffuseFactor = dot(normal, posToLight);
		diffuseFactor *= diffuseFactor;
		//if(diffuseFactor > 0.0)
			lightsContribution += lightCol[i] * diffuseFactor;
	}

	c.r += lightsContribution.r;
	c.g += lightsContribution.g;
	c.b += lightsContribution.b;
	c.a = 1.0f;

	gl_FragColor = c;
}