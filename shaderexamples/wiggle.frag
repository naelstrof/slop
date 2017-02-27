#version 120

uniform sampler2D texture;
uniform vec2 screenSize;
uniform float time;

varying vec2 uvCoord;

void main()
{
    // Higher strength means bigger wobble
    float strength = 10;
    float flatness = 4;
    // Higher speed means faster wobble
    float speed = 2;
    float variation = cos(time);

    float x = uvCoord.x + (sin( time*speed + uvCoord.y/flatness * screenSize.y/strength ) + 0.5)/screenSize.x*strength;
    float y = uvCoord.y + variation*(cos( time*speed + uvCoord.x/flatness * screenSize.x/strength ) + 0.5)/screenSize.y*strength;
    gl_FragColor = texture2D( texture, vec2( x, y ) );
}
