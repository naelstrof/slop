#version 120

uniform sampler2D texture;
uniform sampler2D desktop;

varying vec2 uvCoord;

void main() {
    vec2 uv = vec2(uvCoord.x, -uvCoord.y);
    vec4 color = texture2D(desktop,uv);
    vec4 rect = texture2D(texture,uvCoord);
    gl_FragColor = vec4( 1.0 - color.rgb, color.a )*rect;
}
