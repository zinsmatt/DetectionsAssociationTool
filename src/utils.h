//=========================================================================
//
// Copyright 2019
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Dense>

#include "detection.h"

bool isPointInsideRotatedRect(const Eigen::Vector2d p, const Eigen::Vector2d a,
                              const Eigen::Vector2d b, const Eigen::Vector2d c,
                              const Eigen::Vector2d d);

Eigen::Matrix3d convertEllipseToDualMatrix(const Detection* det);

#endif // UTILS_H
