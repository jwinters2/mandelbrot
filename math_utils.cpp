#include "math_utils.h"

cdouble add(const cdouble& a, const cdouble& b)
{
  cdouble retval;
  retval.r = a.r + b.r;
  retval.i = a.i + b.i;
  return retval;
}

cdouble mul(const cdouble& a, int m)
{
  cdouble retval;
  retval.r = m * a.r;
  retval.i = m * a.i;
  return retval;
}

cdouble square(const cdouble& a)
{
  cdouble retval;
  retval.r = (a.r * a.r) - (a.i * a.i);
  retval.i = 2 * a.r * a.i;
  return retval;
}

double square_magnitude(const cdouble& a)
{
  return (a.r * a.r) + (a.i * a.i);
}

std::string pad_number(int length, int n)
{
  std::string retval = "";  
  while(n > 0)
  {
    retval.insert(0, 1, '0' + (n%10));
    n /= 10;
  }
  while(retval.size() < length)
  {
    retval.insert(0, 1, '0');
  }

  return retval;
}
