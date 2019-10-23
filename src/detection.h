#ifndef DETECTION_H
#define DETECTION_H

#include <string>

struct Detection
{
  float x, y;
  float w, h;
  float a;
  int classe;

  std::string serialize() const;
};

#endif // DETECTION_H
