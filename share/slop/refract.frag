#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform int desktopWidth;
uniform int desktopHeight;
uniform float time;

varying vec2 uvCoord;

vec2 getDesktopUV( vec2 uv ) {
    // Desktop image is upside-down, blame X11 lmao
    uv.y = -uv.y;
    return uv;
}

const float pi = 3.14159265f;

void main() {
    float sigma = 5;
    float numBlurPixelsPerSide = 2.0f;
    float blurMultiplier = 2.0f;

    // Don't change anything below.
    float blurSize = 1.f/desktopHeight*blurMultiplier;
    vec2  blurMultiplyVec      = vec2(0.0f, 1.0f);
    // Higher strength means bigger wobble
    float strength = 8;
    // Higher speed means faster wobble
    float speed = 1;
    float x = (sin( time*speed + uvCoord.y * desktopHeight/strength ) + 0.5)/desktopWidth*strength;
    float y = (cos( time*speed + uvCoord.x * desktopWidth/strength ) + 0.5)/desktopHeight*strength;
    x = x - (sin( time*speed*0.5 + uvCoord.y * desktopHeight/strength ) + 0.5)/desktopWidth*strength/2;
    y = y - (cos( time*speed*0.5 + uvCoord.x * desktopWidth/strength ) + 0.5)/desktopHeight*strength/2;
    vec2 offset = vec2(x,y);

    // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
    vec3 incrementalGaussian;
    incrementalGaussian.x = 1.0f / (sqrt(2.0f * pi) * sigma);
    incrementalGaussian.y = exp(-0.5f / (sigma * sigma));
    incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

    vec4 avgValue = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float coefficientSum = 0.0f;

    // Take the central sample first...
    vec4 color = texture2D( desktop, getDesktopUV( uvCoord.xy ) + offset );
    color.a = texture2D( texture, uvCoord.xy ).a;
    avgValue += color * incrementalGaussian.x;
    coefficientSum += incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;

    // Go through the remaining 8 vertical samples (4 on each side of the center)
    for (float i = 1.0f; i <= numBlurPixelsPerSide; i++) {
        vec2 uv = (uvCoord.xy - i * blurSize * blurMultiplyVec);
        color = texture2D( desktop, getDesktopUV( uv ) + offset );
        color.a = texture2D( texture, uv ).a;
        avgValue += color * incrementalGaussian.x;
        uv = (uvCoord.xy + i * blurSize * blurMultiplyVec);
        color = texture2D( desktop, getDesktopUV( uv ) + offset );
        color.a = texture2D( texture, uv ).a;
        avgValue += color * incrementalGaussian.x;
        coefficientSum += 2 * incrementalGaussian.x;
        incrementalGaussian.xy *= incrementalGaussian.yz;
    }

    //Reset
    blurSize = 1.f/desktopWidth*blurMultiplier;
    blurMultiplyVec      = vec2(1.0f, 0.0f);
    incrementalGaussian.x = 1.0f / (sqrt(2.0f * pi) * sigma);
    incrementalGaussian.y = exp(-0.5f / (sigma * sigma));
    incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

    // Go through the remaining 8 horizontal samples (4 on each side of the center)
    for (float i = 1.0f; i <= numBlurPixelsPerSide; i++) {
        vec2 uv = (uvCoord.xy - i * blurSize * blurMultiplyVec);
        vec4 color = texture2D( desktop, getDesktopUV( uv ) + offset );
        color.a = texture2D( texture, uv ).a;
        avgValue += color * incrementalGaussian.x;
        uv = (uvCoord.xy + i * blurSize * blurMultiplyVec);
        color = texture2D( desktop, getDesktopUV( uv ) + offset );
        color.a = texture2D( texture, uv ).a;
        avgValue += color * incrementalGaussian.x;
        coefficientSum += 2 * incrementalGaussian.x;
        incrementalGaussian.xy *= incrementalGaussian.yz;
    }

    gl_FragColor = avgValue / coefficientSum;
}
