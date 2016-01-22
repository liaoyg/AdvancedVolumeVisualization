#version 330
//layout(location = 0)
out vec4 glTexcolor;
in vec2 pos;

uniform vec3 volumeScale;
uniform vec3 volumeDim;
uniform float layer;

uniform sampler3D volumeTex;

vec3 sampleGrad(sampler3D sampler, vec3 coord)
{
        const int offset = 1;
        float dx = textureOffset(sampler, coord, ivec3(offset, 0, 0)).r - textureOffset(sampler, coord, ivec3(-offset, 0, 0)).r;
        float dy = textureOffset(sampler, coord, ivec3(0, offset, 0)).r - textureOffset(sampler, coord, ivec3(0, -offset, 0)).r;
        float dz = textureOffset(sampler, coord, ivec3(0, 0, offset)).r - textureOffset(sampler, coord, ivec3(0, 0, -offset)).r;
        return vec3(dx, dy, dz);
}

bool IsCoefPoint(float posValue, out int dic)
{
    if(posValue <= 0.33)
    {
        dic = -1;
        return true;
    }
    else if(posValue > 0.33 && posValue <= 0.66)
    {
        dic = 0;
        return true;
    }
    else if(posValue > 0.66)
    {
        dic = 1;
        return true;
    }
}

void main(void)
{

    glTexcolor = vec4(0.0);
    float z = (layer+0.5)/volumeDim.z;

    vec3 pointPos = vec3(pos, z);
    float deltaX = 3.0/volumeDim.x;
    float deltaY = 3.0/volumeDim.y;
    float deltaZ = 3.0/volumeDim.z;
    vec3 delta = vec3(deltaX,deltaY,deltaZ);

    vec3 zerop;
    zerop.x = floor(pointPos.x*volumeDim.x*(1.0/3.0))*deltaX;
    zerop.y = floor(pointPos.y*volumeDim.y*(1.0/3.0))*deltaY;
    zerop.z = floor(pointPos.z*volumeDim.z*(1.0/3.0))*deltaZ;

    vec3 localp = (pointPos - zerop)*(1.0/delta);

    int dicX;
    int dicY;
    int dicZ;
    bool enter = false;
    if(IsCoefPoint(localp.x,dicX)&&IsCoefPoint(localp.y,dicY)&&IsCoefPoint(localp.y,dicZ))
    {
        vec3 nearestPos = pointPos - vec3(dicX*deltaX,dicY*deltaY,dicZ*deltaZ)*(1.0/3.0);
        vec4 inten = texture(volumeTex,nearestPos);
        vec3 grad = sampleGrad(volumeTex,nearestPos);
        glTexcolor = vec4((1.0/3)*(dicX*grad.x + dicY*grad.y + dicZ*grad.z),0,0,0)+inten;
    }else{
        glTexcolor = texture(volumeTex, pointPos);
    }
}
