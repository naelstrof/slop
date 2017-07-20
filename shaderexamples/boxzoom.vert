#version 120

attribute vec2 position;
attribute vec2 uv;

varying vec2 uvCoord;

void main()
{
	uvCoord = uv;
	gl_Position = vec4(position,0,1);
}
