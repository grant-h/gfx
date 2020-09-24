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

struct PointLight {
  vec3 worldPos;
  vec3 color;

  vec3 kDiffuse, kSpecular, kAmbient;
};

uniform vec3 Camera;
uniform mat4 M;
uniform mat4 V;
uniform PointLight pointLights[1];
uniform BasicMaterial material;
//uniform bool UseTex;
//uniform float LightFalloff;
//uniform sampler2D Texture[2];

//const vec3 fogColor = vec3(0.1, 0.0,0.14);
//const float FogDensity = 0.55;

vec3 phongLight(PointLight light, vec3 normal, vec3 worldPos, vec3 eye) {

  vec3 N = normal;
  vec3 L = normalize(light.worldPos - worldPos);
  vec3 R = normalize(reflect(-L, N));
  vec3 V = normalize(eye - worldPos);
  // see https://en.wikipedia.org/wiki/Phong_reflection_model#/media/File:Blinn_Vectors.svg

  float dotLN = max(dot(L, N), 0.0);
  float dotRV = max(dot(R, V), 0.0); // dots can be negative, make sure they are clamped

  return light.color * (
      material.ambient * light.kAmbient +
      material.diffuse * dotLN * light.kDiffuse +
      material.specular * light.kSpecular * pow(dotRV, max(material.shininess, 1.0)));
}

void main()
{
  // We only care about normalized vectors (for direction)
  vec3 norm = normalize(vs_normal);
  vec3 color = vec3(0.0);

  color += phongLight(pointLights[0], norm, vs_frag, Camera);

  //float dist = length(Camera - vs_frag);
  //float fogFactor = (80 - dist)/(80 - 20);
  //fogFactor = clamp( fogFactor, 0.0, 1.0 );

  //vec3 lc = mix(fogColor, color, fogFactor);

  frag_color = vec4(color, 1.0);

  // ########### DEBUG ###########
  //frag_color = vec4(norm.rgb, 1.0); // normal debug
  //frag_color = vec4((V * vec4(norm, 0.0)).rgb, 1.0); // normal camera debug
  //frag_color = texture(Texture[0], vs_texcoord); // texture debug
  //frag_color = vec4(vec3(LinearizeDepth(gl_FragCoord.z)/far), 1.0); // visualize depth buffer
}


