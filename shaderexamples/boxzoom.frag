#version 120

uniform sampler2D texture;
uniform sampler2D desktop;
uniform vec2 screenSize;
uniform vec2 mouse;

varying vec2 uvCoord;

void main()
{
    // Adjustable parameters -------
    vec2 boxOffset = vec2(-64,-64);
    vec2 boxSize = vec2(128,128);
    float magstrength = 5;
    vec2 borderSize = vec2(1,1);
    vec4 borderColor = vec4(0,0,0,1);
    bool crosshair = true;
    //------------------------------

    // actual code (don't touch unless you're brave)

    // convert to UV space
    boxOffset = boxOffset/screenSize;
    boxSize = boxSize/screenSize;
    borderSize = borderSize/screenSize;
    // get mouse position in UV space
    vec2 mpos = vec2(mouse.x, -mouse.y)/screenSize + vec2(0,1);
    vec4 color;

    // Check if our current UV is inside our box.
    if ( uvCoord.x < mpos.x+boxOffset.x+boxSize.x &&
         uvCoord.x > mpos.x+boxOffset.x &&
         uvCoord.y > mpos.y+boxOffset.y &&
         uvCoord.y < mpos.y+boxOffset.y+boxSize.y ) {
      // Check if we're actually inside the crosshair area.
      if ( (distance(uvCoord.x, mpos.x+boxOffset.x+boxSize.x/2) <= borderSize.x ||
            distance(uvCoord.y, mpos.y+boxOffset.y+boxSize.y/2) <= borderSize.y) && crosshair ) {
        color = borderColor;
      } else {
        // Calculate where the UV should be.
        vec2 zoomedUV = ((uvCoord-mpos)-(boxOffset+boxSize/2))/magstrength+mpos;
        // The desktop texture is upside-down due to X11
        vec2 zoomedUVFlipped = vec2( zoomedUV.x, -zoomedUV.y );
        // Then change the color to the desktop color to draw, then add on our rectangle on top.
        vec4 rectColor = texture2D( texture, zoomedUV );
        color = mix( texture2D( desktop, zoomedUVFlipped ), rectColor, rectColor.a );
      }
    // Then check if we're in our border size.
    } else if( uvCoord.x <= mpos.x+boxOffset.x+boxSize.x+borderSize.x &&
         uvCoord.x >= mpos.x+boxOffset.x-borderSize.x &&
         uvCoord.y >= mpos.y+boxOffset.y-borderSize.y &&
         uvCoord.y <= mpos.y+boxOffset.y+boxSize.y+borderSize.y ) {
      color = borderColor;
    } else {
      color = texture2D( texture, uvCoord );
    }

    gl_FragColor = color;
}
