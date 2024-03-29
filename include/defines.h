#ifndef _DEFINES_H_
#define _DEFINES_H_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>



#include <iostream>
#include <cassert>

using glm::cross;
using glm::dot;
using glm::inverse;
using glm::mat3;
using glm::mat4;
using glm::pi;
using glm::radians;
using glm::rotate;
using glm::scale;
using glm::translate;
using glm::transpose;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using Eigen::Matrix;
using Eigen::MatrixXf;
using Eigen::VectorXf;
//using Eigen::Vector3f
#define g glm::vec3(0.0f, -9.8f, 0.0f)
#define PI glm::pi<float>()

#define LOG_INFO(msg)                             \
  std::cerr << "[INFO] "                          \
            << "function(" << __FUNCTION__ << ")" \
            << " line " << __LINE__ << ": " << (msg) << std::endl

#define LOG_ERR(msg)                                                 \
  do                                                                 \
  {                                                                  \
    std::cerr << "[ERROR] "                                          \
              << "function(" << __FUNCTION__ << ")"                  \
              << " line " << __LINE__ << ": " << (msg) << std::endl; \
    std::exit(1);                                                    \
  } while (0)

#define UNIMPLEMENTED                            \
  do                                             \
  {                                              \
    std::cerr << "not implemented" << std::endl; \
    assert(false);                               \
  } while (0)
#endif