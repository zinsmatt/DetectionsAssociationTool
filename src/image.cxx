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

#include "image.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <Eigen/Dense>

#include "utils.h"


Image::Image(fs::path path, int init_index)
    : dataset_index(init_index)
{
  name = path.filename().generic_string();
  fullpath = path.generic_string();
  image = cv::imread(fullpath, cv::IMREAD_UNCHANGED);
}

void Image::loadImageDetections(const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line))
  {
    if (line.size() > 0 && line[0] != '#')
    {
      std::stringstream ss;
      ss.str(line);

      Detection det;
      ss >> det.w;
      ss >> det.h;
      ss >> det.x;
      ss >> det.y;
      ss >> det.a;
      det.w *= 2;
      det.h *= 2;
      //ss >> det.classe;

      detections.emplace_back(det);
    }
  }
  file.close();
}

const Detection *Image::findDetectionUnderPosition(int x, int y)
{
  std::vector<int> indices_inside;
  for (int i = 0; i < detections.size(); ++i)
  {
    const auto& det = detections[i];
    Eigen::Vector2d a(-det.w / 2, -det.h / 2);
    Eigen::Vector2d b( det.w / 2, -det.h / 2);
    Eigen::Vector2d c( det.w / 2,  det.h / 2);
    Eigen::Vector2d d(-det.w / 2,  det.h / 2);
    Eigen::Matrix2d rot;
    double angle = det.a;
    rot << std::cos(angle), -std::sin(angle),
           std::sin(angle), std::cos(angle);
    a = rot * a;
    b = rot * b;
    c = rot * c;
    d = rot * d;
    Eigen::Vector2d p(x - det.x, y - det.y);
    if (isPointInsideRotatedRect(p, a, b, c, d))
    {
      indices_inside.push_back(i);
    }
  }
  if (indices_inside.size() == 0)
    return nullptr;

  std::vector<float> sqdist_to_center;
  for (auto id : indices_inside)
  {
    const auto& det = detections[id];
    sqdist_to_center.push_back(std::pow(det.x - x, 2) + std::pow(det.y - y, 2));
  }

  auto min_dist_index = indices_inside[std::distance(sqdist_to_center.begin(),
                                                     std::min_element(sqdist_to_center.begin(),
                                                                      sqdist_to_center.end()))];
  return &detections[min_dist_index];
}

std::string Image::getText() const
{
    return name + " (" + std::to_string(dataset_index) + ")";
}
