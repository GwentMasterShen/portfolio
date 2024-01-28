#ifndef _UTILS_H_
#define _UTILS_H_
#include "defines.h"


struct WindowGuard final
{
  WindowGuard(GLFWwindow *&, const int width, const int height,
              const std::string &title);
  ~WindowGuard();
};

#endif