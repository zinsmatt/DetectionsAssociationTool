#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Dense>

#include "detection.h"

bool isPointInsideRotatedRect(const Eigen::Vector2d p, const Eigen::Vector2d a,
                              const Eigen::Vector2d b, const Eigen::Vector2d c,
                              const Eigen::Vector2d d);

Eigen::Matrix3d convertEllipseToDualMatrix(const Detection* det);

#endif // UTILS_H
