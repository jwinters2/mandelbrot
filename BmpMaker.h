#ifndef _BMPMAKER_H_
#define _BMPMAKER_H_

class BmpMaker
{
  public:
    BmpMaker(int, int);
    ~BmpMaker();

    int saveToFile(const char*) const;
    int getFilesize() const;

    unsigned char getHeaderByte(int) const;

    void setPixel(int, int, int);
    int getPixel(int, int) const;

    int hsv_to_rgb(int,float,float) const;

  private:
    // the header has two parts, so the total length is 54
    static const int header_size = 14 + 40;
    // the size of the image (not including the header
    const int image_size;
    // an array to generate the header into
    unsigned char header[header_size];

    const int width;
    const int height;
    // the width in pixels must be a multiple of 4, so we might need
    // to adjust the positions of pixels in memory to make that the case
    const int real_width;
    char* image_data;

    void copyIntToHeader(int, int);

    bool isLittleEndian() const;
};

#endif
