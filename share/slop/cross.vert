#version 120

attribute vec2 vertex;
attribute vec2 uv;

varying vec2 uvCoord;

void main()
{
    uvCoord = uv;
	gl_Position = vec4(vertex,0,1);
}
