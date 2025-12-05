#ifndef FREEUTIL_H
#define FREE_UTIL

#include <GL/glew.h>
#include "../Shaderer.h"

void FreeShader(int size, GLuint* buffers, shader* shader) {
  glDeleteBuffers(size, buffers);
  glDeleteVertexArrays(size, buffers);
  Shader_Destroy(shader);
};

#endif
