#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform int desktopWidth;
uniform int desktopHeight;
uniform float time;

varying vec2 uvCoord;

void main() {
    vec2 tc = uvCoord.xy;
    vec2 p = -1.0 + 2.0 * tc;
    float len = length(p);
    vec2 uv = tc + (p/len)*cos(len*12.0-time*4.0)*0.02;
    gl_FragColor = texture2D(texture,uv);
}
