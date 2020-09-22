#version 330 core

in vec4 vs_color;
in vec3 vs_frag;
in vec3 vs_normal;
in vec2 vs_texcoord;
in float vs_height;

uniform vec3 Camera;
uniform mat4 M;
uniform mat4 V;
uniform bool UseTex;
uniform float Mod;
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

const float near = 0.1; 
const float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

const vec3 fogColor = vec3(0.1, 0.0,0.14);
const float FogDensity = 0.55;

void main()
{
  // lighting variables
  float diffuseIntensity = 0.8*(1.0+Mod*0.5);
  float specularIntensity = 0.2*(1.0+Mod*0.5);
  float ambientIntensity = 0.3*(1.0+Mod*0.5);

  // We only care about normalized vectors (for direction)
  vec3 norm = normalize(vs_normal);

  // We are in world space (W - W = W)
  // [.] -----> (camera)
  vec3 eye = normalize(Camera - vs_frag);

  // [.] -----> (light)
  vec3 fragToLight = normalize(light[0].position - vs_frag);
  float fragDistToLight = distance(light[0].position, vs_frag);
  float K = LightFalloff;

  float uvangle = -3.14159265/2.0;
  mat2 uvrot = mat2(cos(uvangle), -sin(uvangle), sin(uvangle), cos(uvangle));
  vec2 uv = vec2(1.0-vs_texcoord.x, vs_texcoord.y);

  if (uv.x < 0.60 && uv.x > 0.55)
    discard;

  // albedo
  vec3 vertexColor = vs_color.rgb;
  vec3 albedo = UseTex ? texture(Texture[0], vs_texcoord).rgb : vertexColor;

  // ambient
  vec3 ambientColor = lightColor;
  vec3 ambient = ambientColor * ambientIntensity;

  // diffuse
  float diff = max(dot(fragToLight, norm), 0.0);
  vec3 diffuse = diffuseIntensity * diff * lightColor;

  // specular
  vec3 reflection = reflect(-fragToLight, norm);
  float spec = pow(clamp(dot(eye, reflection), 0.0, 1.0), 16);
  vec3 specular = specularIntensity * spec * lightColor;

  vec3 color = (ambient + diffuse + specular) * albedo;
  color *= pow(2.73, -fragDistToLight*K);


  float dist = length(Camera - vs_frag);
  float fogFactor = (80 - dist)/(80 - 20);
  fogFactor = clamp( fogFactor, 0.0, 1.0 );

  vec3 lc = mix(fogColor, color, fogFactor);

  frag_color = vec4(lc, 1.0);

  // ########### DEBUG ###########
  //frag_color = vec4(norm.rgb, 1.0); // normal debug
  //frag_color = vec4((V * vec4(norm, 0.0)).rgb, 1.0); // normal camera debug
  //frag_color = texture(Texture[0], vs_texcoord); // texture debug
  //frag_color = vec4(vec3(LinearizeDepth(gl_FragCoord.z)/far), 1.0); // visualize depth buffer
}


