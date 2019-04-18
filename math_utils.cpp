#include "math_utils.h"

/*
 * adds two complex numbers
 */
cdouble add(const cdouble& a, const cdouble& b)
{
  cdouble retval;
  retval.r = a.r + b.r;
  retval.i = a.i + b.i;
  return retval;
}

/*
 * multiplies a complex number by an integer
 */
cdouble mul(const cdouble& a, int m)
{
  cdouble retval;
  retval.r = m * a.r;
  retval.i = m * a.i;
  return retval;
}

/*
 * returns the square of the input
 */
cdouble square(const cdouble& a)
{
  /*
   * c = a + bi
   * c^2 = (a + bi)(a + bi)
   *     = a(a + bi) + bi(a + bi)
   *     = a^2 + abi + abi + (b^2)(i^2)
   *     = a^2 + 2abi + -(b^2)
   *     = (a^2 - b^2) + (2ab)i
   */
  cdouble retval;
  retval.r = (a.r * a.r) - (a.i * a.i);
  retval.i = 2 * a.r * a.i;
  return retval;
}

/*
 * returns the square of the magnitude
 */
double square_magnitude(const cdouble& a)
{
  return (a.r * a.r) + (a.i * a.i);
}

/*
 * returns the number as a string with zeros padded at the front
 *
 * length : the (minimum) number of digits the final number should have
 * n : the numver to pad
 */
std::string pad_number(int length, int n)
{
  // start with an empty string
  std::string retval = "";  

  while(n > 0)
  {
    // insert the last digit of n into the first position of the string
    retval.insert(0, 1, '0' + (n%10));
    // remove the last digit of n
    n /= 10;
  }
  while(retval.size() < length)
  {
    // add zeros to the front until it's big enough
    retval.insert(0, 1, '0');
  }

  return retval;
}
