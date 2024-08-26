#ifndef IMAGE_H
#define IMAGE_H

class image {
  public:
    image(int w, int h);

    ~image();

    uint32_t& operator [] (int index);
    uint32_t  operator [] (int index)     const;
    uint32_t& operator () (int x, int y);
    uint32_t  operator () (int x, int y)  const;
        
  public:
    uint32_t* pixels;
    int width;
    int height;
};




inline image::image(int w, int h): width(w), height(h) {
  pixels = new uint32_t[w*h];
  for (int i = 0; i < w*h; ++i)
    pixels[i] = 0x00000000;
}

inline image::~image() {
  delete[] pixels;
}

inline uint32_t& image::operator [] (int index) {
  return pixels[index];
}

inline uint32_t image::operator [] (int index) const {
  return pixels[index];
}

inline uint32_t& image::operator () (int x, int y) {
  return pixels[y*width + x];
}

inline uint32_t image::operator () (int x, int y) const {
  return pixels[y*width + x];
}


#endif