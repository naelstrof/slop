#version 130

uniform sampler2D texture;

varying vec2 uvCoord;

out vec4 outColor;

void main()
{
    outColor = texture2D( texture, uvCoord );
}

