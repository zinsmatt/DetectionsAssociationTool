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

#ifndef OBJECT_H
#define OBJECT_H

#include <QString>

#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "detection.h"

class Object
{
public:
  Object(int i, int n);

  void setObservation(int index, const Detection& det);
  void removeObservation(int index);

  QString getText() const;

  const Detection* getObservation(int image_idx);

  Eigen::MatrixX3d getObservationsMatrix() const;

  std::string serialize() const;

  void setId(int i) { id = i; }
  int getId() const { return id; }

private:
  int id;
  std::vector<std::unique_ptr<Detection>> observations;
};

#endif // OBJECT_H
