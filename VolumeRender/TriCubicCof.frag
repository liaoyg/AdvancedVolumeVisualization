#version 330
//layout(location = 0)
out vec4 accum;
in vec2 pos;

uniform vec3 volumeScale;
uniform int layer;

uniform sampler3D volumeTex;
uniform sampler2D preIntTex;


mat3 rotationMatrix(float xangle, float yangle, float zangle)
{
//    float sinx = sin(angle)
    mat3 xrotation = mat3(1.0, 0.0, 0.0,
                          0.0, cos(xangle), sin(xangle),
                          0.0, sin(xangle), -cos(xangle));
    mat3 yrotation = mat3(cos(yangle), 0.0, -sin(yangle),
                          0.0, 1.0, 0.0,
                          sin(yangle), 0.0, cos(yangle));
    mat3 zrotation = mat3(cos(zangle), sin(zangle), 0.0,
                          -sin(zangle), cos(zangle), 0.0,
                          0.0, 0.0, 1.0);
    return xrotation*yrotation*zrotation;
}

void main(void)
{
    vec3 boxMin = -volumeScale;
    vec3 boxMax = volumeScale;
    vec3 boxDim = boxMax - boxMin;

    vec3 dir = vec3(1.0,0.0,0.0);
    accum = vec4(0.0);
    float samplesize = 0.002;
    int occSamples = 24;
    int raynumber = 8;
    float z = float((2*layer + 1 )/boxDim.z);
//    float z = float((layer ＋ 0.5)*2/boxDim.z);
    vec3 pointPos = vec3(pos, z);

    accum = clamp(accum, vec4(0.0), vec4(1.0));

    //accum = vec4(1.0,0.0,0.0,1.0);
}
