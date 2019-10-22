#include "object.h"


Object::Object(int i, int n)
  : id(i)
{
  observations.resize(n);
}

void Object::setObservation(int index, const Detection &det) {
  if (index >= 0 && index < observations.size())
  {
    if (observations[index])
      *observations[index] = det;
    else
    {
      observations[index].reset(new Detection(det));
    }
  }
  else
  {
    std::cerr << "Impossible to set observation at image " << index << std::endl;
  }
}

QString Object::getText() const
{
  return QString::fromStdString("Object " + std::to_string(id));
}

const Detection *Object::getObservation(int image_idx)
{
  return observations[image_idx].get();
}
