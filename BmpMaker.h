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
    static const int header_size = 14 + 40;
    const int image_size;
    unsigned char header[header_size];

    const int width;
    const int real_width;
    const int height;
    char* image_data;

    void copyIntToHeader(int, int);

    bool isLittleEndian() const;
};

#endif
