
#pragma once

#include <string>
#include <sstream>

namespace llog
{

template<typename T>
class Argument
{
//  friend std::string to_string(const Argument<T>& argument) const;

//protected:
public:
  const T& unlatched_value_;

public:
  Argument(const T& value) :
    unlatched_value_(value)
  {
  }

  const T& operator*() const
  {
    return unlatched_value_;
  }

  const T* operator->() const
  {
    return &unlatched_value_;
  }


};

template<typename T>
std::string to_string(const Argument<T>& argument)
{
  std::ostringstream os;
  os << argument.unlatched_value_;
  return os.str();
}

}
