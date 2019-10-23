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

#include "object.h"

#include "utils.h"

Object::Object(int i, int n)
  : id(i)
{
  observations.resize(n);
}

void Object::setObservation(int index, const Detection &det) {
  if (index >= 0 && index < observations.size())
  {
    if (observations[index])
      *observations[index] = det;
    else
    {
      observations[index].reset(new Detection(det));
    }
  }
  else
  {
    std::cerr << "Impossible to set observation at image " << index << std::endl;
  }
}

QString Object::getText() const
{
  return QString::fromStdString("Object " + std::to_string(id));
}

const Detection *Object::getObservation(int image_idx)
{
    return observations[image_idx].get();
}

Eigen::MatrixX3d Object::getObservationsMatrix() const
{
    Eigen::MatrixX3d mat(observations.size() * 3, 3);
    for (int i = 0; i < observations.size(); ++i)
    {
        mat.block<3, 3>(i*3, 0) = convertEllipseToDualMatrix(observations[i].get());
    }
    return mat;
}

std::string Object::serialize() const
{
    std::stringstream ss;
    ss << id << "\n";
    for (int i = 0; i < observations.size(); ++i)
    {
        if (observations[i])
        {
            ss << observations[i]->serialize() << "\n";
        }
        else
        {
            ss << "-1 -1 -1 -1 -1 -1\n";
        }
    }
    return ss.str();
}
