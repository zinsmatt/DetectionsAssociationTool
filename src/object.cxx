#include "object.h"

#include "utils.h"

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

Eigen::MatrixX3d Object::getObservationsMatrix() const
{
    Eigen::MatrixX3d mat(observations.size() * 3, 3);
    for (int i = 0; i < observations.size(); ++i)
    {
        mat.block<3, 3>(i*3, 0) = convertEllipseToDualMatrix(observations[i].get());
    }
    return mat;
}

std::string Object::serialize() const
{
    std::stringstream ss;
    ss << id << "\n";
    for (int i = 0; i < observations.size(); ++i)
    {
        if (observations[i])
        {
            ss << observations[i]->serialize() << "\n";
        }
        else
        {
            ss << "-1 -1 -1 -1 -1 -1\n";
        }
    }
    return ss.str();
}
