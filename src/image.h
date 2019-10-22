#ifndef IMAGE_H
#define IMAGE_H

#include <filesystem>

#include <opencv2/opencv.hpp>

#include "detection.h"


namespace fs = std::filesystem;

struct Image
{
  Image(fs::path path);

  cv::Mat image;
  std::string name;
  std::string fullpath;
  std::vector<Detection> detections;

  void loadDetectionsFile(const std::string& filename);
  const Detection* findDetectionUnderPosition(int x, int y);

};

#endif // IMAGE_H
