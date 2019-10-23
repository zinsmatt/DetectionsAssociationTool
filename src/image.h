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

  void loadDetectionsFile(const std::string& filename);
  const Detection* findDetectionUnderPosition(int x, int y);
  std::string getText() const;

};

#endif // IMAGE_H
