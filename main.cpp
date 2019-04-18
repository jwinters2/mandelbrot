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

const int max_threads = 4;
pthread_mutex_t* mutexes = nullptr;
pthread_mutex_t* arg_mutex = nullptr;

void print_progress(int,int);

int main(int argc, char** argv)
{
  const int width = (argc > 1 ? atoi(argv[1]) : 2000);
  const int height = (argc > 2 ? atoi(argv[2]) : 2000);
  const int dimension = (width > height ? width : height);
  std::string filename = (argc > 3 ? std::string(argv[3]) : "mandelbrot");


  int start_frame = 0;
  int num_frames = 60;
  int antialias = 1;
  for(int i = 4; i < argc; i++)
  {
    if(strcmp(argv[i],"--start-frame") == 0)
    {
      start_frame = atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"--num-frames") == 0)
    {
      num_frames = atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"--anti-alias") == 0)
    {
      antialias = atoi(argv[++i]);
    }
  }

  int max_adjust = width / 100;

  BmpMaker* bm[max_threads];
  for(int i=0; i<max_threads; i++)
  {
    bm[i] = new BmpMaker(width, height);
  }
  cdouble pivot;
  cdouble delta_pivot;
  pivot.r = -0.5;
  pivot.i = 0;
  //pivot.r = -0.001643721971153;
  //pivot.i =  0.822467633298876;
  //pivot.r = -0.8838028818148707; // was originally negative
  //pivot.i = 0.2353181904738867;
  delta_pivot.r = 0.0;
  delta_pivot.i = 0.0;

  int p_int = 1;
  long p_iter = 0;
  print_progress(0,num_frames);

  // multithreadded stuff
  int next_thread = 0;

  pthread_t threads[max_threads];
  //pthread_mutex_t mutexes[max_threads];
  int pids[max_threads];
  RenderFrameArgs args[max_threads];
  pthread_mutex_t mutex_arr[max_threads];
  pthread_mutex_t arg_mutex_real = PTHREAD_MUTEX_INITIALIZER;

  for(int i=0; i<max_threads; i++)
  {
    pids[i] = -1;
    mutex_arr[i] = PTHREAD_MUTEX_INITIALIZER;
  }

  mutexes = mutex_arr;
  arg_mutex = &arg_mutex_real;

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
    
    /*
    while(adjusts < max_adjust && left >= center)
    {
      pivot = add(pivot, mul(delta_pivot, -1) );
      center = left;
      left   = mandelbrot( add(pivot, mul(delta_pivot, -1) ) );
      adjusts ++;
    }

    while(adjusts < max_adjust && right >= center)
    {
      pivot = add(pivot, delta_pivot );
      center = right;
      right  = mandelbrot( add(pivot, delta_pivot ) );
      adjusts ++;
    }

    if(mandelbrot(pivot) == -1)
    {
      std::cout << "warning, pivot is in the set" << std::endl;
      pivot = old_pivot;
    }
    */

    if(pids[next_thread] != -1)
    {
      pthread_join(threads[next_thread], nullptr);
      print_progress(++p_iter, num_frames);
    }
    
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

    pthread_create(&threads[next_thread], nullptr, 
                   renderFrame, (void*)&(args[next_thread]));

    pids[next_thread] = 0;
    next_thread = (next_thread + 1) % max_threads;
  }

  for(int i=0; i<max_threads; i++)
  {
    if(pids[i] != -1)
    {
      pthread_join(threads[i], nullptr);
    }
  }

  std::cout << std::endl;

  for(int i=0; i<max_threads; i++)
  {
    delete bm[i];
  }

  return 0;
}

void print_progress(int p, int pmax)
{
  std::cout << "[";
  for(int i=1; i<50; i++)
  {
    if(i%5 == 0)
    {
      std::cout << ((i*pmax/50)<=p ? '#' : '|');
    }
    else
    {
      std::cout << ((i*pmax/50)<=p ? '=' : ' ');
    }
  }
  std::cout << "]  " << (p*100/pmax) << "." << ((p/10)%10) << p%10
            << "%\r"<< std::flush;
}

