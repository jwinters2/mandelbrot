#ifndef __THREAD_H_
#define __THREAD_H_

#include <iostream>

#include "BmpMaker.h"
#include "cdouble.h"

// a struct for the arguments to pass into a new thread
struct RenderFrameArgs
{
  int thread_id;  // which thread-slot it's in
  int iteration;  // which number frame it has to generate
  int antialias;  // how much to smooth out the image 
                  // (1 = none at all, higher = more)

  int width;      // dimensions of the image
  int height;
  int dimension;  // equal to width or height, whichever is larger
                  // this is so the image doesn't get stretched when making
                  // a non-square image

  int start_frame;      // which frame the program started on
  std::string filename; // the beginning of the filename to save to
  cdouble pivot;        // the point we're zooming in on
  BmpMaker* bm;         // a pointer to this thread slot's .bmp maker object
};

//extern pthread_mutex_t* mutexes;

// renderFrame needs a reference to this variable
extern pthread_mutex_t* arg_mutex;

void* renderFrame(void*);

#endif
