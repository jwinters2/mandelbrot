#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <pthread.h>

#include "cdouble.h"
#include "thread.h"
#include "mandelbrot.h"
#include "math_utils.h"
#include "BmpMaker.h"

const int max_threads = 4;      // the maximum number of threads to have at once
pthread_mutex_t* arg_mutex = nullptr;   // a mutex for the arguments object
                                        // to give to each thread

void print_progress(int,int);

int main(int argc, char** argv)
{
  // parse manditory command line arguments
  const int width = (argc > 1 ? atoi(argv[1]) : 2000);
  const int height = (argc > 2 ? atoi(argv[2]) : 2000);
  // dimension is just the greater of either the width or the height
  // in order to keep the scale when rendering to a non-square image
  const int dimension = (width > height ? width : height);
  std::string filename = (argc > 3 ? std::string(argv[3]) : "mandelbrot");

  // parse the optinal arguments
  int start_frame = 0;
  int num_frames = 60;
  int antialias = 1;
  for(int i = 4; i < argc; i++)
  {
    if(strcmp(argv[i],"--start-frame") == 0 || strcmp(argv[i],"-s") == 0)
    {
      start_frame = atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"--num-frames") == 0 || strcmp(argv[i],"-f") == 0)
    {
      num_frames = atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"--anti-alias") == 0 || strcmp(argv[i],"-a") == 0)
    {
      antialias = atoi(argv[++i]);
    }
  }

  // the maximum amount to adjust the pivot point (in pixels) each frame
  int max_adjust = width / 200;

  // setup a .bmp image maker for each frame
  BmpMaker* bm[max_threads];
  for(int i=0; i<max_threads; i++)
  {
    bm[i] = new BmpMaker(width, height);
  }

  // make a pivot point to center or zooming on
  cdouble pivot;
  cdouble delta_pivot;  // delta_pivot is the distance of one pixel
                        // this depends on how deep in the zoom we are
  pivot.r = -0.001643721971153;
  pivot.i =  0.822467633298876;
  //pivot.r = -0.8838028818148707; // was originally negative
  //pivot.i = 0.2353181904738867;
  delta_pivot.r = 0.0;
  delta_pivot.i = 0.0;

  // print progress stuff
  long p_iter = 0; // progress iterator
  print_progress(0,num_frames); // print zero progress to start

  // multithreadded stuff
  int next_thread = 0;

  // each thread has associated with it ...
  pthread_t threads[max_threads]; // ... a thread object ...
  bool thread_used[max_threads];  // ... whether or not the thread was used ...
  RenderFrameArgs args[max_threads];  // ... and a set of arguments

  // the arguments need a mutex (I think, I was having a race condition bug,
  // and I never tested the program without this after I fixed it)
  pthread_mutex_t arg_mutex_real = PTHREAD_MUTEX_INITIALIZER;

  for(int i=0; i<max_threads; i++)
  {
    thread_used[i] = false;
  }

  arg_mutex = &arg_mutex_real;

  // for each frame we want to render
  for(int iteration = start_frame;
      iteration < start_frame + num_frames;
      iteration++)
  {
    // adjust pivot so that it's on the border of the mandelbrot set
    delta_pivot.r = 6.0 * pow(1.01,-iteration) / dimension;
    cdouble old_pivot = pivot;
    int center = mandelbrot(pivot);
    int left   = mandelbrot( add(pivot, mul(delta_pivot, -1) ) );
    int right  = mandelbrot( add(pivot, delta_pivot ) );
    int adjusts = 0;
    
    // shift it left until we hit the border
    while(adjusts < max_adjust && left >= center)
    {
      pivot = add(pivot, mul(delta_pivot, -1) );
      center = left;
      left   = mandelbrot( add(pivot, mul(delta_pivot, -1) ) );
      adjusts ++;
    }

    // shift it right until we hit the border
    while(adjusts < max_adjust && right >= center)
    {
      pivot = add(pivot, delta_pivot );
      center = right;
      right  = mandelbrot( add(pivot, delta_pivot ) );
      adjusts ++;
    }

    // adjusting only works if the pivot is outside the set
    if(mandelbrot(pivot) == -1)
    {
      //std::cerr << "warning, pivot is in the set" << std::endl;
      pivot = old_pivot;
    }

    // if there's a thread already running in this slot, we need to wait for it
    if(thread_used[next_thread])
    {
      pthread_join(threads[next_thread], nullptr);
      // update the progess bar
      if(num_frames > 10)
      {
        print_progress(++p_iter, num_frames);
      }
    }
    
    // copy over the command line arguments
    // (I know it's messy, this was originally all in this loop until
    // I decided to make it multi-threaded
    pthread_mutex_lock(arg_mutex);

    args[next_thread].thread_id = next_thread;
    args[next_thread].iteration = iteration;
    args[next_thread].antialias = antialias;
    args[next_thread].width = width;
    args[next_thread].height = height;
    args[next_thread].dimension = dimension;
    args[next_thread].filename = filename;
    args[next_thread].start_frame = start_frame;
    args[next_thread].pivot = pivot;
    args[next_thread].bm = bm[next_thread];

    pthread_mutex_unlock(arg_mutex);

    // spawn a new thread
    pthread_create(&threads[next_thread], nullptr, 
                   renderFrame, (void*)&(args[next_thread]));

    // mark this thread slot used
    thread_used[next_thread] = true;
    // move over to the next thread slot
    next_thread = (next_thread + 1) % max_threads;
  }

  // join all the leftover threads (but only if they were used,
  // otherwise it segfaults
  for(int i=0; i<max_threads; i++)
  {
    if(thread_used[i])
    {
      pthread_join(threads[i], nullptr);
    }
  }

  std::cout << std::endl;

  // cleanup the .bmp image makers
  for(int i=0; i<max_threads; i++)
  {
    delete bm[i];
  }

  return 0;
}

/* prints the progress bar
 *
 * p : the number of frames currently done
 * pmax : the total number of frames to do
 */
void print_progress(int p, int pmax)
{
  std::cout << "[";
  for(int i=1; i<50; i++)
  {
    if(i%5 == 0)
    {
      // print interval markers if i is a multiple of 5
      std::cout << ((i*pmax/50)<=p ? '#' : '|');
    }
    else
    {
      std::cout << ((i*pmax/50)<=p ? '=' : ' ');
    }
  }
  
  // adjusted progress in ten-thousands of the way done
  // so we can print the percent to two decimal places
  // )and not bother learn how to do this the right way with floats)
  int ap = (p*10000)/pmax;
  std::cout << "]  " << (ap/100) << "." << ((ap/10)%10) << ap%10
            << "%\r"<< std::flush;

  // "\r" returns to the beginning of the line, but doesn't go to the next
  // one, so we overwrite the current line over and over again for progress
}

