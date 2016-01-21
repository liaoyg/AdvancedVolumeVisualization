#version 330

in vec3 CameraDir;
in vec3 CameraPos;

out vec4 FragColor;

uniform vec3 volumeDim;
uniform vec3 volumeScale;
uniform mat4 invModelView;
uniform sampler3D volumeTex;
uniform sampler3D LAOTex;
uniform sampler2D transferFuncTex;
uniform sampler2D preInt;

uniform int lighttype;
uniform vec3 ambient;
uniform vec3 lightcolor;
uniform vec3 posdir;
uniform float fshiness;
uniform float fstrength;
uniform float fcosattent;
uniform float flineattent;
uniform float fquadattent;
uniform float ambientCoE;
uniform float specularC;
uniform float diffuseC;
uniform float OCSize;

uniform float azimuth, elevation, clipPlaneDepth; //clipping plane variables
uniform bool clipFlag; //clipping on or off
uniform bool preintFlag;
uniform bool LAOFlag;

uniform float deltaStep;

vec3 lightPos = vec3(1.0, 1.0, 0.0);
bool intersectBox(vec3 ori, vec3 dir, vec3 boxMin, vec3 boxMax, out float t0, out float t1)
{
	vec3 invDir = 1.0 / dir;
	vec3 tBot = invDir * (boxMin.xyz - ori);
	vec3 tTop = invDir * (boxMax.xyz - ori);

	vec3 tMin = min(tTop, tBot);
	vec3 tMax = max(tTop, tBot);

	vec2 temp = max(tMin.xx, tMin.yz);
	float tMinMax = max(temp.x, temp.y);
	temp = min(tMax.xx, tMax.yz);
	float tMaxMin = min(temp.x, temp.y);

	bool hit;
	if((tMinMax > tMaxMin))
		hit = false;
	else
		hit = true;

	t0 = tMinMax;
	t1 = tMaxMin;

	return hit;
}

vec3 sampleGrad(sampler3D sampler, vec3 coord)
{
	const int offset = 1;
	float dx = textureOffset(sampler, coord, ivec3(offset, 0, 0)).r - textureOffset(sampler, coord, ivec3(-offset, 0, 0)).r;
	float dy = textureOffset(sampler, coord, ivec3(0, offset, 0)).r - textureOffset(sampler, coord, ivec3(0, -offset, 0)).r;
	float dz = textureOffset(sampler, coord, ivec3(0, 0, offset)).r - textureOffset(sampler, coord, ivec3(0, 0, -offset)).r;
	return vec3(dx, dy, dz);
}

vec3 p2cart(float azimuth,float elevation)
{
    float pi = 3.1415926;
    float x, y, z, k;
    float ele = -elevation * pi / 180.0;
    float azi = (azimuth + 90.0) * pi / 180.0;

    k = cos( ele );
    y = sin( ele );
    z = sin( azi ) * k;
    x = cos( azi ) * k;

    return vec3( x, y, z );
}

void clipping(vec3 start, vec3 end, vec3 dir, out vec3 newstart, out vec3 newend, out vec3 newdir)
{
    float len = length(end-start);
    vec3 clipPlane = p2cart(azimuth, elevation);
    //next, see if clip plane faces viewer
    bool frontface = (dot(dir , clipPlane) > 0.0);
    //next, distance from ray origin to clip plane
    float dis = dot(dir,clipPlane);
    if (dis != 0.0  )  dis = (-clipPlaneDepth - dot(clipPlane, start.xyz-0.5)) / dis;
    if ((!frontface) && (dis < 0.0))
    {
        newstart = start;
        newend = end;
        newdir = dir;
        return;
    }
    if ((frontface) && (dis > len))
    {
        newstart = start;
        newend = end;
        newdir = dir;
        return;
    }
    if ((dis > 0.0) && (dis < len))
    {
        if (frontface)
        {
            newstart = start + dir * dis;
        }
        else
        {
            newend =  start + dir * dis;
        }
        newdir = newend - newstart;
        len = length(newdir);
    }
}

float getLightIntensity(vec3 norm, vec3 pos)
{
    //vec3 dir = normalize(pos);
    float LightIntensity = 1.0f;
    // L->
    vec3 lightVec = normalize(lightPos - pos);
    // V->
    vec3 viewVec = normalize(CameraPos-pos);
    //R->
    vec3 reflectvec = -reflect(lightVec,norm);
    //calculate Ambient Term:
    float Iamb = ambientCoE;
    if(LAOFlag)
        Iamb = texture(LAOTex,pos).r*ambientCoE;
    //calculate Diffuse Term:
    float Idiff =   diffuseC * max(dot(norm, lightVec), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);
    // calculate Specular Term:
    float Ispec = specularC*pow(max(dot(reflectvec,viewVec),0.0),0.3*fshiness);
    Ispec = clamp(Ispec, 0.0, 1.0);
    return (Idiff+Iamb+Ispec);
}

float BernsteinForm(float x, int order, int level)
{
    if(order > level)
        return 1;
    if(level == 3)
    {
        if(order == 0)
            return (1-x)*(1-x)*(1-x);
        else if(order == 1)
            return 3*(1-x)*(1-x)*x;
        else if(order == 2)
            return 3*(1-x)*x*x;
        else if(order == 3)
            return x*x*x;
    }
    else if (level == 2)
    {
        if(order == 0)
            return (1-x)*(1-x);
        else if(order == 1)
            return 2*(1-x)*x;
        else if(order == 2)
            return x*x;
    }
    else
        return 0;
}

void triCubicSample(vec3 pos, sampler3D sampler, out float intensity, out vec3 gradient)
{
   vec3 volumeSize = volumeDim;
   float deltaX = 1.0/volumeSize.x;
   float deltaY = 1.0/volumeSize.y;
   float deltaZ = 1.0/volumeSize.z;
   vec3 delta = vec3(deltaX,deltaY,deltaZ);

//   pos = pos + vec3(0.0001,0.0001,0.0001);
   vec3 zerop;
   zerop.x = floor(pos.x*volumeSize.x)*deltaX;
   zerop.y = floor(pos.y*volumeSize.y)*deltaY;
   zerop.z = floor(pos.z*volumeSize.z)*deltaZ;
//   intensity = zerop.z;
//   return;

//   float b[100];
//   const int offset = 1;
   int i,j,k,x,y,z;
//   for(i = 0; i<2; i++)
//   {
//       for(j = 0; j<2; j++)
//       {
//           for(k = 0; k<2;k++)
//           {
//               int dicX = i*(-2)+1;
//               int dicY = j*(-2)+1;
//               int dicZ = k*(-2)+1;
////               vec3 coordPoint = zerop+vec3(deltaX*i,deltaY*j,deltaZ*k);
//               vec3 posp = zerop + vec3(deltaX*i, deltaY*j, deltaZ*k);
//               float intens = texture(sampler, posp).r;
//               b[i*3+j*3*4+k*3*16] = intens;
//               vec3 grad = sampleGrad(sampler, posp);
//               for(x = 0; x<2; x++)
//                   for(y = 0; y<2; y++)
//                       for(z = 0; z<2; z++)
//                       {
//                           b[i*3+dicX*x+(j*3+dicY*y)*4+(k*3+dicZ*z)*16] = intens + 0.3333*(x*dicX*grad.x+y*dicY*grad.y+z*dicZ*grad.z);
//                       }
//           }
//        }
//   }
   vec3 localp = (pos - zerop - 0.001)*(1.0/delta);
//   if(localp.x >= 0 && localp.x <= 1)
//   {
//       intensity = localp.z;
//       return;
//   }
//   intensity = 0;
   for(i = 0; i<4; i++)
   {
       for(j =0; j<4; j++)
       {
           for(k = 0; k<4;k++)
           {
//               intensity += b[i+j*4+k*16]*BernsteinForm(localp.x,i,3)*BernsteinForm(localp.y,j,3)*BernsteinForm(localp.z,k,3);
               vec3 bpos = zerop + 0.333*vec3(i*deltaX,j*deltaY,k*deltaZ);
               float bvalue = texture(sampler, bpos).r;
               intensity += bvalue*BernsteinForm(localp.x,i,3)*BernsteinForm(localp.y,j,3)*BernsteinForm(localp.z,k,3);
           }
       }
   }

//   gradient = sampleGrad(sampler, pos);
   for(i = 0; i<3; i++)
   {
       for(j =0; j<4; j++)
       {
           for(k = 0; k<4;k++)
           {
               vec3 b1pos = zerop + vec3((i+1)*deltaX,j*deltaY,k*deltaZ);
               float b1value = texture(sampler, b1pos).r;
               vec3 b2pos = zerop + vec3(i*deltaX,j*deltaY,k*deltaZ);
               float b2value = texture(sampler, b2pos).r;
               gradient.x += (b1value - b2value)*BernsteinForm(localp.x,i,2)*BernsteinForm(localp.y,j,3)*BernsteinForm(localp.z,k,3);
           }
       }
   }

   for(i = 0; i<4; i++)
   {
       for(j =0; j<3; j++)
       {
           for(k = 0; k<4;k++)
           {
               vec3 b1pos = zerop + vec3(i*deltaX,(j+1)*deltaY,k*deltaZ);
               float b1value = texture(sampler, b1pos).r;
               vec3 b2pos = zerop + vec3(i*deltaX,j*deltaY,k*deltaZ);
               float b2value = texture(sampler, b2pos).r;
               gradient.y += (b1value - b2value)*BernsteinForm(localp.x,i,3)*BernsteinForm(localp.y,j,2)*BernsteinForm(localp.z,k,3);
           }
       }
   }

   for(i = 0; i<4; i++)
   {
       for(j =0; j<4; j++)
       {
           for(k = 0; k<3;k++)
           {
               vec3 b1pos = zerop + vec3(i*deltaX,j*deltaY,(k+1)*deltaZ);
               float b1value = texture(sampler, b1pos).r;
               vec3 b2pos = zerop + vec3(i*deltaX,j*deltaY,k*deltaZ);
               float b2value = texture(sampler, b2pos).r;
               gradient.z += (b1value - b2value)*BernsteinForm(localp.x,i,3)*BernsteinForm(localp.y,j,3)*BernsteinForm(localp.z,k,2);
           }
       }
   }
}


void main(void)
{
        vec3 boxMin = -volumeScale;
        vec3 boxMax = volumeScale;
	vec3 boxDim = boxMax - boxMin;

	// discard if ray-box intersection test fails
	vec3 dir = normalize(CameraDir);
	float t0, t1;
	bool hit = intersectBox(CameraPos, dir, boxMin, boxMax, t0, t1);
	if(!hit)
		discard;

	// discard if the volume is behind the camera
	t0 = max(t0, 0.0);
        if(t1 < t0)
        {
                discard;
        }
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);

        vec3 rayStart = CameraPos + CameraDir*t0;
        vec3 rayEnd = CameraPos + CameraDir*t1;
        bool isfrontbordor = false;
        float originalLenth = length(rayEnd - rayStart);

        rayStart = (1.0/boxDim)*(rayStart - boxMin); //transform to (0,1)
        rayEnd = (1.0/boxDim)*(rayEnd - boxMin);

        vec4 col_acc = vec4(0,0,0,0); // The dest color
        vec3 rayDir = rayEnd - rayStart;
        float len = length(rayDir);
        rayDir = normalize(rayDir);

        //clipping
        vec3 clipPlane = p2cart(azimuth, elevation);

        if (clipFlag)
            {
                //next, see if clip plane faces viewer
                bool frontface = (dot(rayDir , clipPlane) > 0.0);
                //next, distance from ray origin to clip plane
                float dis = dot(rayDir,clipPlane);
                if (dis != 0.0  )
                    dis = (-clipPlaneDepth - dot(clipPlane, rayStart.xyz-0.5)) / dis;
                if ((!frontface) && (dis < 0.0))
                    return;
                if ((frontface) && (dis > len))
                    return;
                if ((dis > 0.0) && (dis < len))
                {
                    if (frontface)
                    {
                        rayStart = rayStart + rayDir * dis;
                    }
                    else
                    {
                        rayEnd =  rayStart + rayDir * dis;
                    }
                rayDir = rayEnd - rayStart;
                len = length(rayDir);
                rayDir = normalize(rayDir);
                }
            }

        //lighting
        vec3 Ambient = ambient;
        vec3 LightColor = lightcolor;
        //vec3 LightDirection = vec3(1.0,1.0,0.0); // direction toward the light
        vec3 LightPosition = posdir;
        vec3 HalfVector = vec3(0.8,0.6,0.0);
        float Shininess = fshiness;
        float Strength = fstrength;

        float ConstantAttenuation = fcosattent; // attenuation coefficients
        float LinearAttenuation = flineattent;
        float QuadraticAttenuation = fquadattent;

        float delta = deltaStep;
        float alpha_acc = 0.0;                // The  dest alpha for blending
        vec4 color_sample = vec4(0,0,0,0); // The src color

        vec3 step = normalize(rayDir) * delta;
        float preStep = texture(volumeTex,rayStart).r;
        rayStart += step;
        //bool preIntFlag = pre;
        float step_acc = t0;
        int i=0;
        for ( i=0; i < 2000 ; ++i, rayStart += step)
        {

            // sampling
//            float currentStep = texture(volumeTex,rayStart).r;
            float intensity;
            vec3 gradient;
            triCubicSample(rayStart,volumeTex,intensity,gradient);
            intensity = clamp(intensity, 0.0,1.0);
            float currentStep = intensity;
            col_acc = vec4(gradient.x,0,0,1.0);
            break;
            // classify
            if(!preintFlag)
                color_sample = texture(transferFuncTex,vec2(currentStep,0.0));
            else
                color_sample = texture(preInt,vec2(preStep,currentStep));

            //lighting
            float laoValue = texture(LAOTex,rayStart).r;
//            Ambient = Ambient * laoValue;
//            if(lighttype == 0)
//            {
//                vec3 Normal = sampleGrad(volumeTex,rayStart);
//                float diffuse = max(0.0, dot(Normal, LightPosition));
//                float specular = max(0.0, dot(Normal, HalfVector));
//                // surfaces facing away from the light (negative dot products)
//                // won’t be lit by the directional light
//                if (diffuse == 0.0)
//                   specular = 0.0;
//                else
//                    specular = pow(specular, Shininess);
//                // sharpen the highlight
//                vec3 scatteredLight = Ambient + LightColor * diffuse;
//                vec3 reflectedLight = LightColor * specular * Strength;

//                color_sample.rgb = min(color_sample.rgb * scatteredLight + reflectedLight, vec3(1.0));
//            }
//            else if(lighttype == 1)
//            {
//                vec3 Normal = sampleGrad(volumeTex,rayStart);
//                vec3 lightDirection = LightPosition - vec3(rayStart);
//                float lightDistance = length(lightDirection);
//                lightDirection = lightDirection / lightDistance;

//                float attenuation = 1.0 /
//                                 (ConstantAttenuation +
//                                    LinearAttenuation * lightDistance +
//                                 QuadraticAttenuation * lightDistance * lightDistance);
//                HalfVector = normalize(lightDirection + CameraDir);

//                float diffuse = max(0.0, dot(Normal, lightDirection));
//                float specular = max(0.0, dot(Normal, HalfVector));

//                if (diffuse == 0.0)
//                    specular = 0.0;
//                else
//                    specular = pow(specular, Shininess)*Strength;
//                vec3 scatteredLight = Ambient + LightColor * diffuse * attenuation;
//                vec3 reflectedLight = LightColor * specular * attenuation;
//                color_sample.rgb = min(color_sample.rgb * scatteredLight + reflectedLight, vec3(1.0));
//            }
            if(lighttype == 3)
            {
//                float lightIntensity = getLightIntensity(sampleGrad(volumeTex,rayStart), rayStart);
                float lightIntensity = getLightIntensity(gradient, rayStart);
                color_sample.rgb *=lightIntensity;
            }

            //opacity correction
            if(!preintFlag)
            {
                color_sample.a = 1 - pow(1 - color_sample.a, deltaStep/OCSize);
                color_sample.rgb *= color_sample.a;
            }
            else
            {
                preStep = currentStep;
            }

            col_acc += (1.0 - col_acc.a) * color_sample;
            //alpha_acc += color_sample.a*(1 - alpha_acc);
            step_acc += delta;
            if(step_acc >= (t1 - delta))
                break;
            if(col_acc.a > 0.999)
                 break;
        }

        col_acc = clamp (col_acc, vec4(0.0), vec4(1.0));
        FragColor = col_acc;
}
