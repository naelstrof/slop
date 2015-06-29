#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform int desktopWidth;
uniform int desktopHeight;
uniform float time;

varying vec2 uvCoord;

void main()
{
    // Higher strength means bigger wobble
    float strength = 8;
    // Higher speed means faster wobble
    float speed = 2;
    float x = uvCoord.x + (sin( time*speed + uvCoord.y * desktopHeight/strength ) + 0.5)/desktopWidth*strength;
    float y = uvCoord.y + (cos( time*speed + uvCoord.x * desktopWidth/strength ) + 0.5)/desktopHeight*strength;
    gl_FragColor = texture2D( texture, vec2( x, y ) );
}
