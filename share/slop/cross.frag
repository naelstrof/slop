#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform int desktopWidth;
uniform int desktopHeight;
uniform float time;

varying vec2 uvCoord;

void main() {
    float stitching_size = 6.0;
    int invert = 0;

    float size = stitching_size;
    vec2 cPos = uvCoord * vec2(desktopWidth, desktopHeight);
    vec2 tlPos = floor(cPos / vec2(size, size));
    tlPos *= size;
    int remX = int(mod(cPos.x, size));
    int remY = int(mod(cPos.y, size));
    if (remX == 0 && remY == 0)
    tlPos = cPos;
    vec2 blPos = tlPos;
    blPos.y += (size - 1.0);
    if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {
        if (invert == 1) {
            gl_FragColor = vec4(0.2, 0.15, 0.05, 1.0);
        } else {
            gl_FragColor = texture2D(texture, tlPos * vec2(1.0/desktopWidth, 1.0/desktopHeight)) * 1.4;
        }
    } else {
        if (invert == 1) {
            gl_FragColor = texture2D(texture, tlPos * vec2(1.0/desktopWidth, 1.0/desktopHeight)) * 1.4;
        } else {
            gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        }
    }
}
