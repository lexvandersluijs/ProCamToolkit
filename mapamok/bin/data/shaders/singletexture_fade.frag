#version 130
#extension GL_ARB_texture_rectangle : enable

#define PI (3.1415926536)
#define TWO_PI (6.2831853072)

uniform sampler2DRect background;

uniform float fadeFactor = 1.0;

void main() 
{
	vec4 c = vec4(0.0, 0.0, 0.0, 1.0);

	vec2 pos = gl_TexCoord[0].st * 2048.0;

	// simply copy background, with fadefactor
	vec4 bgColor = vec4(texture2DRect( background, pos ).rgb * fadeFactor, 1.0);

	gl_FragColor = bgColor;
}
