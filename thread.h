#ifndef __THREAD_H_
#define __THREAD_H_

#include <iostream>

#include "BmpMaker.h"
#include "cdouble.h"

struct RenderFrameArgs
{
  int thread_id;
  int iteration;
  int antialias;
  int width;
  int height;
  int dimension;
  int start_frame;
  std::string filename;
  cdouble pivot;
  BmpMaker* bm;
};

extern pthread_mutex_t* mutexes;
extern pthread_mutex_t* arg_mutex;

void* renderFrame(void*);

#endif
