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

    vec2 tc = uvCoord.xy;
    vec2 p = -1.0 + 2.0 * tc;
    float len = length(p);
    vec2 offset = (p/len)*cos(len*12.0-time*4.0)*0.005;

    // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
    float blurSize = 1.f/desktopWidth*blurMultiplier;
    vec2 blurMultiplyVec      = vec2(0.0f, 1.0f);
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
