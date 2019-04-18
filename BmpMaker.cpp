#include "BmpMaker.h"

#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>

//const char header[54];
//const int** image_data;

BmpMaker::BmpMaker(int w, int h)
                  :width(w),height(h),
                  //effective width must be a multiple of 4
                  real_width( w%4 == 0 ? w : w + 4 - (w%4)),  
                  image_size(3 * h * ( w%4 == 0 ? w : w + 4 - (w%4)))
{
  memset(header, 0, header_size);

  // the header starts with BM
  memcpy(header, "BM", 2);

  // next 4 bytes are the file size
  copyIntToHeader(2, header_size + image_size);
  // offset from the start of the file to where the first pixel is
  copyIntToHeader(10, header_size);

  //unsigned int bitmap_size = 40;
  //memcpy(&(header[14]), &bitmap_size, 4);

  header[14] = 40;  // the size of BITMAPINFOHEADER is 40
  copyIntToHeader(18, width);  //  width of the image
  copyIntToHeader(22, height); // height of the image
  header[26] = 1;   // number of color planes (must be 1)
  header[28] = 24;  // bits per pixel (24)

  // initalize image data
  image_data = new char[image_size];
  memset(image_data, 0, image_size);
}

BmpMaker::~BmpMaker()
{
  delete [] image_data;
}

int BmpMaker::saveToFile(const char* filename) const
{
  std::ofstream outFile;
  outFile.open(filename);
  if(outFile.is_open())
  {
    outFile.write((char*)header, header_size);
    outFile.write((char*)image_data, image_size); 
    outFile.close();
    return 0;
  }
  else
  {
    // file failed to open
    return -1;
  }
}

int BmpMaker::getFilesize() const
{
  return header_size + image_size;
}

void BmpMaker::setPixel(int x, int y, int color)
{
  if(x >= 0 && x < width && y >= 0 && y < height)
  {
    color << 8;
    int pos = 3 * ((real_width * y) + x);
    memcpy(&image_data[pos], &color, 3);
  }
}

int BmpMaker::getPixel(int x, int y) const
{
  if(x >= 0 && x < width && y >= 0 && y < height)
  {
    int retval = 0;
    int pos = 3 * ((real_width * y) + x);
    memcpy(&retval, &image_data[pos], 3);
    retval >> 8;
    return retval;
  }
  else
  {
    return -1;
  }
}

unsigned char BmpMaker::getHeaderByte(int b) const
{
  if(b >= 0 && b < header_size)
  {
    return header[b];
  }

  return '_';
}

void BmpMaker::copyIntToHeader(int pos, int value)
{
  if(isLittleEndian())
  {
    memcpy(&(header[pos]), &value, 4);
  }
  else
  {
    // little endian means we need to swap the order of the bytes
    char buffer[4];
    memcpy(buffer, &value, 4);
    header[pos  ] = buffer[3];
    header[pos+1] = buffer[2];
    header[pos+2] = buffer[1];
    header[pos+3] = buffer[0];
  }
}

bool BmpMaker::isLittleEndian() const
{
  unsigned int i = 1;
  char* c = (char*)(&i);
  return (*c == 1);
}

int BmpMaker::hsv_to_rgb(int hue, float sat, float val) const
{
  hue = hue % 360;
  float strength = sat * val;
  float intensity = (1.0f - sat) * val;
  float variable = strength * (1-fabs( fmod(float(hue)/60.0f, 2.0f) - 1 ));

  float r, g, b;

  if(hue < 60)
  {
    r = strength;
    g = variable;
    b = 0.0f;
  }
  else if(hue < 60 * 2)
  {
    r = variable;
    g = strength;
    b = 0.0f;
  }
  else if(hue < 60 * 3)
  {
    r = 0.0f;
    g = strength;
    b = variable;
  }
  else if(hue < 60 * 4)
  {
    r = 0.0f;
    g = variable;
    b = strength;
  }
  else if(hue < 60 * 5)
  {
    r = variable;
    g = 0.0f;
    b = strength;
  }
  else
  {
    r = strength;
    g = 0.0f;
    b = variable;
  }

  int retval = (0x10000 * (int)((r + intensity) * 0xFF))
             + (  0x100 * (int)((g + intensity) * 0xFF))
             + (    0x1 * (int)((b + intensity) * 0xFF));

  return retval;
}
