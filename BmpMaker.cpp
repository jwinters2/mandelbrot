#include "BmpMaker.h"

#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>

//const char header[54];
//const int** image_data;

/*
 * Constructor
 *
 * w : width of the image in pixels
 * h : height of the image in pixels
 */
BmpMaker::BmpMaker(int w, int h)
                  :width(w),height(h),
                  //effective width must be a multiple of 4
                  real_width( w%4 == 0 ? w : w + 4 - (w%4)),  
                  // 3 * width * height
                  image_size(3 * h * ( w%4 == 0 ? w : w + 4 - (w%4)))
{
  // zero out the header
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

/*
 * Destructor
 *
 * Just needs to free the image data
 */
BmpMaker::~BmpMaker()
{
  // free the image data
  delete [] image_data;
}

/*
 * saves the image data to a file
 *
 * filename : name of the file to save to (.bmp is not appended)
 */
int BmpMaker::saveToFile(const char* filename) const
{
  // make and open a file
  std::ofstream outFile;
  outFile.open(filename);

  // if it successfully opened
  if(outFile.is_open())
  {
    // write the header data to it
    outFile.write((char*)header, header_size);
    // write the image data to it
    outFile.write((char*)image_data, image_size); 
    // close it
    outFile.close();
    return 0;
  }
  else
  {
    // file failed to open
    return -1;
  }
}

/*
 * returns the file size in bytes
 */
int BmpMaker::getFilesize() const
{
  return header_size + image_size;
}

/*
 * sets a pixel to a color
 *
 * x,y : the coordinates of the pixel
 * color : the color to set it to
 */
void BmpMaker::setPixel(int x, int y, int color)
{
  // if the coordinates are valid
  if(x >= 0 && x < width && y >= 0 && y < height)
  {
    // the color is in the format 0x00abcdef
    // we need to shift it over a bit
    color << 8;

    // get the position in memory of this pixel
    int pos = 3 * ((real_width * y) + x);

    // copy the memory over (we're working with 3 bytes per pixel here)
    memcpy(&image_data[pos], &color, 3);
  }
}

/*
 * returns the color value of a specific pixel
 *
 * x,y : the coordinates of the pixel
 * 
 * returns : the color of that pixel
 */
int BmpMaker::getPixel(int x, int y) const
{
  // if the coordinates are valid
  if(x >= 0 && x < width && y >= 0 && y < height)
  {
    // declare a variable to copy the data into
    int retval = 0;

    // get the position in memory of this pixel
    int pos = 3 * ((real_width * y) + x);

    // copy the memory over into retval
    memcpy(&retval, &image_data[pos], 3);

    // shift it over a bit to make easier to work with
    retval >> 8;

    return retval;
  }
  else
  {
    // the pixel is outside of the image
    return -1;
  }
}

/*
 * get a specific byte of the header
 *
 * b : which byte to get
 */
unsigned char BmpMaker::getHeaderByte(int b) const
{
  // if b is a valid index 
  if(b >= 0 && b < header_size)
  {
    // return 
    return header[b];
  }

  // this function is just a way for me to test the header-setup code
  // this explains the strange choice for error-return-value
  return '_';
}

/*
 * copy an int to a position into the header
 *
 * pos : where to put it
 * value : the int to put into pos
 *
 * I didn't happen to need a copyShortIntoHeader function
 */
void BmpMaker::copyIntToHeader(int pos, int value)
{
  // .bmp images require all numbers in the header to be little endian
  if(isLittleEndian())
  {
    // if the memory/cpu also little endian, we can just copy over
    memcpy(&(header[pos]), &value, 4);
  }
  else
  {
    // the memory/cpu is big endian, so we need to flip the order of the bytes

    // declare a buffer and copy the int into it to get each byte
    // there's probably a way to do this with unions, but whatever
    char buffer[4]; 
    memcpy(buffer, &value, 4);

    // copy it over, swapping the order
    header[pos  ] = buffer[3];
    header[pos+1] = buffer[2];
    header[pos+2] = buffer[1];
    header[pos+3] = buffer[0];
  }
}

/*
 * checks if the memory/cpu is little endian
 */
bool BmpMaker::isLittleEndian() const
{
  // make an int
  unsigned int i = 1;
  // cast it to a char array
  char* c = (char*)(&i);
  // the least significant bit first means it's little endian
  return (*c == 1);
}

/*
 * convert a set of HSV to a single, RGB hex number
 */
int BmpMaker::hsv_to_rgb(int hue, float sat, float val) const
{
  hue = hue % 360;

  // these are just formulas I got from the internet
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

  // convert each RGB component into a single hex number
  int retval = (0x10000 * (int)((r + intensity) * 0xFF))
             + (  0x100 * (int)((g + intensity) * 0xFF))
             + (    0x1 * (int)((b + intensity) * 0xFF));

  return retval;
}
