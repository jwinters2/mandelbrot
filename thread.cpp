#include "thread.h"

#include <cmath>
#include <iostream>

#include "math_utils.h"
#include "mandelbrot.h"

/*
 * renders a single frame of the mandelbrot animation
 *
 * data : RenderFrameArgs* : the set of arguments to use for this frame
 *
 * returns : nothing
 */
void* renderFrame(void* data)
{
  // make a local copy the argument object so we can let go of the mutex ASAP
  pthread_mutex_lock(arg_mutex);
  RenderFrameArgs a = *((RenderFrameArgs*) data);
  pthread_mutex_unlock(arg_mutex);
  
  // for each pixel in the image ...
  for(int y = 0; y < a.height; y++)
  {
    for(int x = 0; x < a.width; x++)
    {
      // (setup some variables)
      int total_hue = 0;
      int num_in_set = 0;
      cdouble c;

      // ... for each sub-pixel value ...
      for(int xa = a.antialias * x; xa < a.antialias * (x+1); xa++)
      {
        for(int ya = a.antialias * y; ya < a.antialias * (y+1); ya++)
        {
          // ... calculate the actual coordinate of that point
          c.r = (double(xa-(a.antialias*a.width/2))  * 6.0 * pow(1.01,-a.iteration)
                / (a.dimension * a.antialias)) + a.pivot.r;
          c.i = (double(ya-(a.antialias*a.height/2)) * 6.0 * pow(1.01,-a.iteration)
                / (a.dimension * a.antialias)) + a.pivot.i;

          // calculate if it's in the mandelbrot set
          int m = mandelbrot(c);
          if(m >= 0)
          {
            // it's in the set, so save the pixel's color
            num_in_set++;
            total_hue += m;
          }
        }
      }

      // set the color to the average hue and value
      int color = a.bm->hsv_to_rgb(5*total_hue/(a.antialias*a.antialias), 1,
                                ((double)num_in_set)/(a.antialias*a.antialias));
      // color that pixel                   
      a.bm->setPixel(x,y,color); 
    }
  }

  // append the number and ".bmp" to the filename and save it
  std::string real_fn = a.filename;
  real_fn.append(pad_number(4,a.iteration - a.start_frame));
  real_fn.append(".bmp");
  a.bm->saveToFile(real_fn.c_str());

  return nullptr;
}
