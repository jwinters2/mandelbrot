#include "thread.h"

#include <cmath>
#include <iostream>

#include "math_utils.h"
#include "mandelbrot.h"

void* renderFrame(void* data)
{
  pthread_mutex_lock(arg_mutex);
  RenderFrameArgs a = *((RenderFrameArgs*) data);
  pthread_mutex_unlock(arg_mutex);

  pthread_mutex_lock(&mutexes[a.thread_id]);
  
  for(int y = 0; y < a.height; y++)
  {
    for(int x = 0; x < a.width; x++)
    {
      int total_hue = 0;
      int num_in_set = 0;
      cdouble c;

      for(int xa = a.antialias * x; xa < a.antialias * (x+1); xa++)
      {
        for(int ya = a.antialias * y; ya < a.antialias * (y+1); ya++)
        {
          c.r = (double(xa-(a.antialias*a.width/2))  * 6.0 * pow(1.01,-a.iteration)
                / (a.dimension * a.antialias)) + a.pivot.r;
          c.i = (double(ya-(a.antialias*a.height/2)) * 6.0 * pow(1.01,-a.iteration)
                / (a.dimension * a.antialias)) + a.pivot.i;
          int m = mandelbrot(c);
          if(m >= 0)
          {
            num_in_set++;
            total_hue += m;
          }
        }
      }

      int color = a.bm->hsv_to_rgb(5*total_hue/(a.antialias*a.antialias), 1,
                                ((double)num_in_set)/(a.antialias*a.antialias));
      a.bm->setPixel(x,y,color); 
    }
  }

  std::string real_fn = a.filename;
  real_fn.append(pad_number(4,a.iteration - a.start_frame));
  real_fn.append(".bmp");
  a.bm->saveToFile(real_fn.c_str());

  pthread_mutex_unlock(&mutexes[a.thread_id]);

  return nullptr;
}
