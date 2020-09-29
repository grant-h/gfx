#version 330 core

in vec4 vs_color;
in vec3 vs_frag;
in vec3 vs_normal;
in vec2 vs_texcoord;

out vec4 frag_color;

struct BasicMaterial {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct MultiLight {
  int type;

  vec3 worldPos;

  vec3 color;
  float kDiffuse, kSpecular, kAmbient;

  vec3 direction; // directional and spotlight only
  float radius; // point light only
  float cutoff; // spotlight
  float outerCutoff; // spotlight
};

uniform int numLights;
uniform int TextureMode;
uniform vec3 Camera;
uniform mat4 M;
uniform mat4 V;
uniform MultiLight lights[10];
uniform BasicMaterial material;
//uniform bool UseTex;
//uniform float LightFalloff;
uniform sampler2D TextureDiffuse;
uniform sampler2D TextureSpecular;

//const vec3 fogColor = vec3(0.1, 0.0,0.14);
//const float FogDensity = 0.55;
//
const float screenGamma = 2.2; // sRGB

vec3 attenuateStandard(vec3 color, float dist, float kLinearAtt, float kQuadAtt)
{
  return color * (1.0 / (1.0 + dist * kLinearAtt + dist * dist * kQuadAtt));
}

// From this awesome article https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
// The cutoff parameter will clamp attenuation values below it to zero. Small values recommended
vec3 attenuateRadius(vec3 color, float dist, float lightRadius, float cutoff)
{
  cutoff = max(min(cutoff, 1.0), 0.0);

  float d = max(dist - lightRadius, 0.0);
  float denom = d / lightRadius + 1.0;
  float attenuation = 1.0 / (denom*denom);
  attenuation = (attenuation - cutoff) / (1.0 - cutoff);
  attenuation = max(attenuation, 0.0);

  return color * attenuation;
}

/*vec3 attenuateSpotlight(vec3 L, vec3 direction, float cutoff)
{
  float theta = dot(L, normalize(direction));

  if (theta > cutoff)
}*/

vec3 blinnPhongLight(BasicMaterial mat, MultiLight light, vec3 normal, vec3 worldPos, vec3 eye) {

  // see https://en.wikipedia.org/wiki/Phong_reflection_model#/media/File:Blinn_Vectors.svg
  vec3 N = normal;
  vec3 L = (light.type == 0) ? normalize(light.worldPos - worldPos) : normalize(-light.direction);
  vec3 V = normalize(eye - worldPos);
  vec3 H = normalize(L + V); // blinn-phong half-angle (looks so much better)

  // dots can be negative, make sure they are clamped
  float dotLN = max(dot(L, N), 0.0);
  float dotRV = max(dot(H, N), 0.0);

  // Old "Phong" only specular
  //vec3 R = normalize(reflect(-L, N));
  //float dotRV = max(dot(R, V), 0.0); // dots can be negative, make sure they are clamped

  vec3 color = light.color * (
      mat.ambient * light.kAmbient +
      mat.diffuse * dotLN * light.kDiffuse +
      mat.specular * light.kSpecular * pow(dotRV, max(mat.shininess, 1.0)));

  return (light.type == 0) ?
    attenuateRadius(color, distance(light.worldPos, worldPos), light.radius, 0.05) :
    color;
}

vec3 spotlight(BasicMaterial mat, MultiLight light, vec3 normal, vec3 worldPos, vec3 eye) {
  vec3 L = normalize(light.worldPos - worldPos);
  float theta = dot(L, normalize(-light.direction));
  float eps = light.cutoff - light.outerCutoff;
  float intensity = clamp((theta - light.outerCutoff) / eps, 0.0, 1.0);

  return blinnPhongLight(mat, light, normal, worldPos, eye) * intensity;
}

void main()
{
  // We only care about normalized vectors (for direction)
  vec3 norm = normalize(vs_normal);
  vec3 color = vec3(0.0);

  BasicMaterial mat;
  mat.diffuse = bool(TextureMode & 1) ? texture(TextureDiffuse, vs_texcoord).rgb : material.diffuse;
  mat.specular = bool(TextureMode & 2) ? texture(TextureSpecular, vs_texcoord).rgb : material.specular;
  mat.ambient = bool(TextureMode & 1) ? mat.diffuse : material.ambient;
  mat.shininess = material.shininess;

  for (int i = 0; i < numLights; i++) {
    if (lights[i].type == 2)
      color += spotlight(mat, lights[i], norm, vs_frag, Camera);
    else
      color += blinnPhongLight(mat, lights[i], norm, vs_frag, Camera);
  }

  vec3 fogColor = vec3(0.5, 0.5, 0.7);
  float dist = length(Camera - vs_frag);
  float fogFactor = (15 - dist)/(80 - 20);
  fogFactor = clamp( fogFactor, 0.0, 1.0 );

  //color = mix(fogColor, color, fogFactor);
  // TODO: investigate texture input files' gamma values (are they sRGB?)
  //frag_color = vec4(pow(color, vec3(1.0 / screenGamma)), 1.0);
  frag_color = vec4(color, 1.0);

  // ########### DEBUG ###########
  //frag_color = vec4(norm.rgb, 1.0); // normal debug
  //frag_color = vec4((V * vec4(norm, 0.0)).rgb, 1.0); // normal camera debug
  //frag_color = vec4(vec3(LinearizeDepth(gl_FragCoord.z)/far), 1.0); // visualize depth buffer
}


