#include "detection.h"

#include <sstream>

std::string Detection::serialize() const
{
    std::stringstream ss;
    ss << x << " " << y << " " << w << " " << h << " " << a << " " << classe;
    return ss.str();
}
