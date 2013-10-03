#include <iostream>

namespace testinga 
{
  
template<class T = double> T inc(T val)
{
  return val++;
}

template<> int inc<int>(int val)
{
  return val++;
}

}