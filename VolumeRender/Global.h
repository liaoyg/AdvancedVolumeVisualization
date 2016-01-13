#ifndef GLOBAL_H
#define GLOBAL_H
#include "Vec.h"

extern std::vector<Vec4f> tfdata;
extern bool gb_Clipflag;
extern float azimuth;
extern float elevation;
extern float clipPlaneDepth;
extern float deltaStep;
extern float opacityCorrection;

extern Vec3f gb_ambient;
extern Vec3f gb_lightcolor;
extern Vec3f gb_PosOrDirect;
extern float gb_Shiness;
extern float gb_LightStrength;
extern float gb_CosAttent;
extern float gb_LineAttent;
extern float gb_QuadAttent;

extern float gb_AmbientCoE;
extern float gb_DiffuseC;
extern float gb_SpecularC;
extern float gb_Shiness;
extern bool gb_AOFlag;

extern bool gb_PreIntFlag;


#endif // GLOBAL_H
