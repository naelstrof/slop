#version 120

uniform sampler2D texture;
uniform vec2 screenSize;
uniform float time;

varying vec2 uvCoord;

const float pi = 3.14159265f;

vec3 color(float x) {
    return max(min(sin(vec3(x,x+pi*2.0/3.0,x+pi*4.0/3.0))+0.5,1.0),0.0);
}

void main() {
    vec2 pos = (( gl_FragCoord.xy / screenSize.xy )-0.5)*screenSize.xy/screenSize.x*4.0;
    pos+=normalize(pos);
    pos.xy+=sin(pos.yx*10.0)*0.1;
    float r=(2.0/(dot(pos,pos)*10.0+1.0));
    vec2 rr=vec2(cos(r),sin(r));
    pos=pos.xy*rr.xx+pos.yx*rr.yy*vec2(-1.0,1.0);
    float f=(length(pos)*10.0)+time;
    //f=acos((pos.x/length(pos)*0.5+0.5)*pi);
    f+=sin(atan(pos.y,pos.x)*7.0)*5.0;
    vec4 rect = texture2D(texture,uvCoord);
    gl_FragColor = vec4(color(f),1.f)*rect;
}
