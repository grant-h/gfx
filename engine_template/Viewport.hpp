#ifndef _VIEWPORT_H
#define _VIEWPORT_H

struct Viewport {
  float x, y;
  float width, height;
  float bgColor_RGB[3];

  glm::vec3 cameraPos;
  glm::vec3 center;
  glm::mat4 view;
  glm::mat4 projection;
};

#endif // _VIEWPORT_H
