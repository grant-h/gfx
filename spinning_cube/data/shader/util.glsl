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
