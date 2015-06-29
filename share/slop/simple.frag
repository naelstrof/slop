#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform float time;

varying vec2 uvCoord;

void main()
{
    gl_FragColor = texture2D( texture, uvCoord );
}
