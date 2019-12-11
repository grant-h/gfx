#version 330 core

in vec4 vs_color;
in vec3 vs_frag;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform vec3 Camera;
uniform mat4 M;
uniform mat4 V;
uniform bool UseTex;
uniform float Mod;
uniform float Time;
uniform float SphereRadius;
uniform float LightFalloff;
uniform sampler2D Texture[2];
uniform vec3 lightPos[2];
uniform vec3 lightColor;

out vec4 frag_color;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform Light light[2];

// https://github.com/jamieowen/glsl-blend/blob/master/_temp/conversion/desaturate.glsl
vec3 Desaturate(vec3 color, float Desaturation)
{
  vec3 grayXfer = vec3(0.3, 0.59, 0.11);
  vec3 gray = vec3(dot(grayXfer, color));
  return vec3(mix(color, gray, Desaturation));
}

const vec3 fogColor = vec3(0.1, 0.0,0.14);
const float FogDensity = 0.55;

void main()
{
  // lighting variables
  //vec3 lightPos = vec3(sin(Time)*2.0, 2.0, cos(Time)*2.0); // world space
  //vec3 lightColor = vec3(0.5 + (sin(Time) + 1.0)*0.5, 0.5, 0.5);
  float diffuseIntensity = 0.8*(1.0+Mod*0.5);
  float specularIntensity = 0.2*(1.0+Mod*0.5);
  float ambientIntensity = 0.3*(1.0+Mod*0.5);

  // We only care about normalized vectors (for direction)
  vec3 norm = normalize(vs_normal);
  // I have no idea what this is doing... but it calculates normals!
  // https://forum.libcinder.org/topic/calculating-normals-after-displacing-vertices-in-shader
  norm = normalize( cross( dFdx( vs_frag.xyz ), dFdy( vs_frag.xyz ) ) );

  // spherical fade in
  float sphere_dist = length(vs_frag - vec3(0.0, 1.0, 0.0)) - SphereRadius;

  // rectangular fade in
  //float sphere_dist = vs_frag.z + 115.0 - SphereRadius*6.0;

  // We are in world space (W - W = W)
  // [.] -----> (camera)
  vec3 eye = normalize(Camera - vs_frag);

  // [.] -----> (light)
  vec3 fragToLight = normalize(light[0].position - vs_frag);
  float fragDistToLight = distance(light[1].position, vs_frag);
  float K = LightFalloff;//(sin(Time*4.0) + 1.0)/4.0; // falloff

  // ambient
  vec3 vertexColor = vs_color.rgb;
  vec3 diffuseColor = UseTex ? mix(texture(Texture[0], vs_texcoord), texture(Texture[1], vs_texcoord), 0.5).rgb : vertexColor;
  vec3 ambientColor = vec3(0.1, 0.1, 0.1);
  vec3 ambient = ambientColor * ambientIntensity;

  // diffuse
  float diff = max(dot(fragToLight, norm), 0.0);
  vec3 diffuse = diffuseIntensity * diff * lightColor;

  // specular
  vec3 reflection = reflect(-fragToLight, norm);
  float spec = pow(clamp(dot(eye, reflection), 0.0, 1.0), 16);

  vec3 specular = specularIntensity * spec * lightColor;

  vec3 color = (ambient + diffuse + specular) * diffuseColor;
  color *= pow(2.73, -fragDistToLight*K);
  //vec3 color2 = (ambient + diffuse + specular) * diffuseColor2;

  // Uncomment if you want normal shading w.r.t camera
  //vec4 normCamera = V * vec4(norm, 0.0);
  //frag_color = vec4(normCamera.rgb, 1.0);

  vec3 lc = mix(color, color*2, Mod);
  //vec3 lc2 = mix(color2, color2*2, Mod);

  //float in_sphere = min(max(sphere_dist, 0.0), 1.0);
  //lc = Desaturate(lc, in_sphere);
  //lc = mix(lc2, lc, in_sphere);

  float dist = length(Camera - vs_frag);
  float fogFactor = (80 - dist)/(80 - 20);
  fogFactor = clamp( fogFactor, 0.0, 1.0 );

  lc = mix(fogColor, lc, fogFactor);

  //frag_color = vec4(lc, 1.0);
  //frag_color = vec4(vec3(, 0.0, 0.0), 1.0);
  //frag_color = vec4(1.0, 0.0,0.0, 1.0);
  frag_color = vec4(lc, 1.0);
  //frag_color = vec4(vec3(1.0, 0.0, 0.0), 1.0);
}
