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

#ifndef DETECTION_H
#define DETECTION_H

#include <string>

struct Detection
{
  // all these parameters are only used for displaying ellipses when we click on a detection.
  // For the association, only the index of the detection in the image is used.
  float x, y;
  float w, h;
  float a;  // radians
  int classe;
  std::string classe_name;


  int in_image_index;   // index of the detection in the image

  std::string serialize() const;
};

#endif // DETECTION_H
