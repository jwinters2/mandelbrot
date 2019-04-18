#include "mandelbrot.h"

#include "math_utils.h"
#include "cdouble.h"

/*
 * checks whether or not c is in the mandelbrot set
 *
 * returns -1 if it is in the set
 * otherwise, returns the number of iterations it takes to be ruled out
 * (this is used to compute the colors)
 */
int mandelbrot(const cdouble& c)
{
  // start with z = 0
  cdouble z;
  z.r = 0.0;
  z.i = 0.0;

  for(int i=0; i<500; i++)
  {
    // z = z^2 + c
    z = add(c,square(z));

    // if the magnitude is > 2, it's outside the set
    if(square_magnitude(z) > 4)
    {
      return i;
    }
  }

  // after 500 iterations, it's not clear yet just assume it's in the set
  return -1;
}
