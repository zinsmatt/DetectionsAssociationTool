#ifndef OBJECT_H
#define OBJECT_H

#include <QString>

#include <iostream>
#include <memory>
#include <vector>

#include <detection.h>

class Object
{
public:
  Object(int i, int n);

  void setObservation(int index, const Detection& det);

  QString getText() const;

  const Detection* getObservation(int image_idx);


private:
  int id;
  std::vector<std::unique_ptr<Detection>> observations;
};

#endif // OBJECT_H