#include "mandelbrot.h"

#include "math_utils.h"
#include "cdouble.h"

int mandelbrot(const cdouble& c)
{
  cdouble z;
  z.r = 0.0;
  z.i = 0.0;

  for(int i=0; i<500; i++)
  {
    z = add(c,square(z));
    if(square_magnitude(z) > 4)
    {
      return i;
    }
  }

  return -1;
}
