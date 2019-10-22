#include "image.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <Eigen/Dense>

bool isPointInsideRotatedRect(const Eigen::Vector2d p,
                              const Eigen::Vector2d a,
                              const Eigen::Vector2d b,
                              const Eigen::Vector2d c,
                              const Eigen::Vector2d d)
{
  std::cout << "points = \n";
  std::cout << "a= " << a.transpose() << "\n";
    std::cout << b.transpose() << "\n";
      std::cout << c.transpose() << "\n";
        std::cout << d.transpose() << "\n";
          std::cout << "p= " << p.transpose() << "\n";
  Eigen::Vector2d pa = a-p;
  Eigen::Vector2d pb = b-p;
  Eigen::Vector2d pc = c-p;
  Eigen::Vector2d pd = d-p;
  double det_ab = pa[0] * pb[1] - pa[1] * pb[0];
  double det_bc = pb[0] * pc[1] - pb[1] * pc[0];
  double det_cd = pc[0] * pd[1] - pc[1] * pd[0];
  double det_da = pd[0] * pa[1] - pd[1] * pa[0];
  std::cout << det_ab << " "
               << det_bc << " "
                  << det_cd << " "
                     << det_da << std::endl;
  if (det_ab > 0 && det_bc > 0 && det_cd > 0 && det_da > 0
      || det_ab < 0 && det_bc < 0 && det_cd < 0 && det_da < 0)
    return true;
  else
    return false;
}


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

const Detection *Image::findDetectionUnderPosition(int x, int y)
{
  std::cout << "===============++> lickc  = " << x << " " << y << std::endl;
  std::vector<int> indices_inside;
  for (int i = 0; i < detections.size(); ++i)
  {
    const auto& det = detections[i];
    Eigen::Vector2d a(-det.w / 2, -det.h / 2);
    Eigen::Vector2d b( det.w / 2, -det.h / 2);
    Eigen::Vector2d c( det.w / 2,  det.h / 2);
    Eigen::Vector2d d(-det.w / 2,  det.h / 2);
    Eigen::Matrix2d rot;
    double angle = 3.1416 * det.a / 180;
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
