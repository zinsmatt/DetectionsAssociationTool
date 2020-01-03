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

#include "utils.h"

#include <iostream>

bool isPointInsideRotatedRect(const Eigen::Vector2d p,
                              const Eigen::Vector2d a,
                              const Eigen::Vector2d b,
                              const Eigen::Vector2d c,
                              const Eigen::Vector2d d)
{
  Eigen::Vector2d pa = a-p;
  Eigen::Vector2d pb = b-p;
  Eigen::Vector2d pc = c-p;
  Eigen::Vector2d pd = d-p;
  double det_ab = pa[0] * pb[1] - pa[1] * pb[0];
  double det_bc = pb[0] * pc[1] - pb[1] * pc[0];
  double det_cd = pc[0] * pd[1] - pc[1] * pd[0];
  double det_da = pd[0] * pa[1] - pd[1] * pa[0];

  if ((det_ab > 0 && det_bc > 0 && det_cd > 0 && det_da > 0)
      || (det_ab < 0 && det_bc < 0 && det_cd < 0 && det_da < 0))
    return true;
  else
    return false;
}

Eigen::Matrix3d convertEllipseToDualMatrix(const Detection *det)
{
    Eigen::Matrix3d mat = Eigen::Matrix3d::Zero();
    if (det)
    {
        mat(0, 0) = 1.0 / std::pow(det->w / 2, 2);
        mat(1, 1) = 1.0 / std::pow(det->h / 2, 2);
        mat(2, 2) = -1.0;
        Eigen::Matrix3d R;
        R << std::cos(det->a), -std::sin(det->a), 0.0,
             std::sin(det->a),  std::cos(det->a), 0.0,
             0.0, 0.0, 1.0;
        Eigen::Matrix3d T;
        T << 1.0, 0.0, det->x,
             0.0, 1.0, det->y,
             0.0, 0.0, 1.0;
        Eigen::Matrix3d T_inv = T.inverse();
        mat = T_inv.transpose() * R * mat * R.transpose() * T_inv;
        Eigen::Matrix3d dual_mat = mat.inverse();
        //dual_mat /= dual_mat(2, 2);
        return dual_mat;
    }
    return mat;
}
