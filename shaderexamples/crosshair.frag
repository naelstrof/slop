#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform vec2 screenSize;
uniform vec2 mouse;

varying vec2 uvCoord;

void main()
{
    // adjustable parameters
    float circleSize = 128;
    float borderSize = 2;
    // The smaller this value is, the more intense the magnification!
    float magnifyNerf = 1.1;
    vec4 borderColor = vec4(0,0,0,1);
    bool crosshair = true;

    // actual code
    vec2 mUV = vec2(mouse.x, -mouse.y)/screenSize + vec2(0,1);
    float du = distance(mUV,uvCoord);
    float dr = distance(mUV*screenSize,uvCoord*screenSize);
    vec4 color = vec4(0);
    if ( dr > circleSize+borderSize ) {
        color = texture2D( texture, uvCoord );
    } else if ( dr < circleSize ) {
        if ( crosshair && (distance(mUV.x, uvCoord.x)<1/screenSize.x || distance(mUV.y,uvCoord.y)<1/screenSize.y) ) {
            color = borderColor;
        } else {
            float t = 1-du;
            vec2 b = uvCoord;
            vec2 c = (mUV-uvCoord);
            vec2 upsideDown = c/magnifyNerf*t*t+b;

            vec4 textureColor = texture2D( texture, upsideDown );
            color = mix( texture2D( desktop, vec2(upsideDown.x, -upsideDown.y) ), textureColor, textureColor.a );
        }
    } else if ( dr < circleSize+borderSize ) {
        color = borderColor;
    }
    gl_FragColor = color;
}
