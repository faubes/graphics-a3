// ==========================================================================
// $Id: lit_boxes_sol.fs,v 1.4 2016/03/12 03:34:42 jlang Exp $
// Draw boxes with instancing
// ==========================================================================
// (C)opyright:
//
//   Jochen Lang
//   EECS, University of Ottawa
//   800 King Edward Ave.
//   Ottawa, On., K1N 6N5
//   Canada.
//   http://www.site.uottawa.ca
//
// Creator: jlang (Jochen Lang)
// Email:   jlang@site.uottawa.ca
// ==========================================================================
// $Log: lit_boxes_sol.fs,v $
// Revision 1.4  2016/03/12 03:34:42  jlang
// Fixed spot light
//
// Revision 1.3  2014/10/18 14:13:57  jlang
// Added core. Runs with OpenGL 3.3 but material block arrays need 4.0 to work correctly.
//
// Revision 1.2  2014/03/15 04:00:03  jlang
// Switch to uniform material loading instaed of UBO.
//
// Revision 1.1  2014/03/01 22:26:07  jlang
// Created lab 6 with material parameters and use of ubo
//
// Revision 1.3  2014/02/23 02:18:41  jlang
// Make sure material index is flat.
//
// Revision 1.2  2014/02/22 19:51:39  jlang
// Drawing with indexed materials and 1 light source
//
// Revision 1.1  2014/02/17 18:52:57  jlang
// Draft lab5
//
// Revision 1.1  2014/02/03 03:38:45  jlang
// Draft lab check in
//
// ==========================================================================
#version 330 core
// following example from https://computergraphics.stackexchange.com/questions/5323/dynamic-array-in-glsl
#define MAX_LIGHTS 5
#define M_PI 3.1415926535897932384626433832795

in vec4 colorVertFrag;
in vec3 normalFrag;
in vec3 eyeFrag;
in vec4 lightFrag[MAX_LIGHTS];
flat in int materialId;


out vec4 color;

struct Material {
  vec4 emissive;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

layout (std140) uniform MaterialBlock {
  uniform Material materials[4];
};


struct LightSource {

  bool isEnabled;
  bool isLocal;
  bool isSpotLight;

  float strength;

  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  // spot light
  // v is the vector to the vertex
  // if dir*v < cos(cutoff) then (dir * v)^N

  vec3 spot_direction;
  float spot_exponent;
  float spot_cutoff;
  // attentuation 1/(k_c + k_l r + k_q r^2)
  // r is the distance of a vertex from the light source
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;
};

uniform LightSource lights[MAX_LIGHTS];

uniform int nLights;

void main() {
  int mI = materialId % 3 + 4; // use only new materials
  vec3 NVec = normalize(normalFrag);
  vec3 EVec = normalize(eyeFrag);
  vec4 scatteredLight = vec4(0.0);
  vec4 reflectedLight = vec4(0.0);

  for (int i=0; i < nLights; i++) {
    if (!lights[i].isEnabled) {
      continue;
    }

      vec3 LVec = normalize(vec3(lightFrag[i]));
      float distanceLight = length(vec3(lightFrag[i]));
      vec3 HVec = normalize(LVec+EVec);

      // cosine of the directions
      // surface normal and light
      float diffuse = max(0.0,dot(NVec,LVec));
      // half vector and normal
      float specular = max(0.0,dot(NVec,HVec));

      float attenuation = 1.0;
      // for local lights, compute attenuation
      if (lights[i].isLocal) {
         attenuation = 1.0 /
              (lights[i].constant_attenuation +
                lights[i].linear_attenuation * distanceLight +
                lights[i].quadratic_attenuation * distanceLight * distanceLight);
          // if spotlight hits outside cosine fall off, set attenuation to 0
          if ( lights[i].isSpotLight ) {
                 float dotSV = dot(-LVec,normalize(lights[i].spot_direction)); // what happens if normalize 0 vec??
                 if ( dotSV < cos(radians(lights[i].spot_cutoff))) {
                   attenuation = 0.0;
               } else {
                    float n = lights[i].spot_exponent;
                   attenuation *= pow(dotSV,n) * (n+2)/(2*M_PI) ;
               }
             } // end spotlight handling
           } // end PointLight handling

      // check if light hits surface
      if (diffuse == 0.0) {
        specular = 0.0;
      }
      else {
        specular = pow(specular, materials[mI].shininess);
      }

      // scattered light
      scatteredLight += lights[i].strength * attenuation * materials[mI].ambient * lights[i].ambient;
      scatteredLight += lights[i].strength * diffuse * attenuation * materials[mI].diffuse * lights[i].diffuse;

      // reflected light
      reflectedLight += lights[i].strength * specular * attenuation * lights[i].specular * materials[mI].specular;
      }
  // color


  color =  min(scatteredLight + reflectedLight, vec4(1.0)); // add material transparency?
  // color = colorVertFrag;
  // color = vec4(LVec,1.0);
}
