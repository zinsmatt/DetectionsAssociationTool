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

#ifndef IMAGE_H
#define IMAGE_H

#include <experimental/filesystem>

#include <opencv2/opencv.hpp>

#include "detection.h"


namespace fs = std::experimental::filesystem;

struct Image
{
  Image(fs::path path, int init_index);

  cv::Mat image;
  std::string name;
  std::string fullpath;
  std::vector<Detection> detections;
  int dataset_index;

  void loadImageDetections(const std::string& filename, const std::string &filename_classes="");

  const Detection* findDetectionUnderPosition(int x, int y);

  std::string getText() const;

};

#endif // IMAGE_H
