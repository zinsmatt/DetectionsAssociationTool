#include "image.h"

#include <iostream>
#include <fstream>
#include <sstream>


Image::Image(fs::path path)
{
  name = path.stem().generic_string();
  fullpath = path.generic_string();
  image = cv::imread(fullpath, cv::IMREAD_UNCHANGED);
}

void Image::loadDetectionsFile(const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line))
  {
    std::cout << "line = " << line << std::endl;
    if (line.size() > 0 && line[0] != '#')
    {
      std::stringstream ss;
      ss.str(line);

      Detection det;
      ss >> det.x;
      ss >> det.y;
      ss >> det.w;
      ss >> det.h;
      ss >> det.a;
      ss >> det.classe;

      detections.emplace_back(det);
    }
  }


  file.close();
}
