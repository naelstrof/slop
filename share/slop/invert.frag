#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform int desktopWidth;
uniform int desktopHeight;
uniform float time;

varying vec2 uvCoord;

void main() {
    vec2 uv = vec2( uvCoord.x, -uvCoord.y );
    vec4 color = texture2D(desktop,uv);
    float alpha = texture2D(texture,uvCoord).a;
    gl_FragColor = vec4( 1.0 - color.r, 1.0 - color.g, 1.0 - color.b, alpha );
}
